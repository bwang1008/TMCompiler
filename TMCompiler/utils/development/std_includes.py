import re
from typing import Dict, Generator, List, Optional, Set, Tuple

from rich.panel import Panel

includes_provides: Dict[str, List[str]] = {
    "algorithm": ["std::max", "std::min"],
    "cctype": ["std::isspace"],
    "chrono": ["std::chrono"],
    "cstddef": ["std::ptrdiff_t", "std::size_t"],
    "ctime": ["std::ctime", "std::time_t"],
    "exception": ["std::exception"],
    "fstream": ["std::ifstream"],
    "iomanip": ["std::setw"],
    "ios": ["std::ios", "std::ios_base", "std::left", "std::right"],
    "iostream": ["std::cout", "std::endl", "std::clog"],
    "list": ["std::list"],
    "map": ["std::map"],
    "ostream": ["std::ostream"],
    "regex": ["std::regex", "std::regex_match", "std::regex_search", "std::smatch"],
    "set": ["std::set"],
    "sstream": ["std::stringstream"],
    "stdexcept": [
        "std::invalid_argument",
        "std::logic_error",
        "std::runtime_error",
        "std::out_of_range",
    ],
    "string": ["std::string", "std::to_string", "std::getline"],
    "string_view": ["std::string_view"],
    "unordered_map": ["std::unordered_map"],
    "unordered_set": ["std::unordered_set"],
    "utility": ["std::make_pair", "std::pair", "std::move"],
    "vector": ["std::vector"],
}

belongs_in_what_header: Dict[str, str] = {
    provided: k for k, v in includes_provides.items() for provided in v
}


def find_all(pattern: str, text: str) -> Generator[int, None, None]:
    start = 0
    while True:
        start = text.find(pattern, start)
        if start == -1:
            break
        yield start
        start += len(pattern)


def what_is_included(lines: List[str]) -> Set[str]:
    includes: Set[str] = set()
    for line in lines:
        if line.startswith("#include"):
            begin: int = line.find("<")
            end: int = line.find(">")

            if begin == -1:
                continue

            header_name: str = line[1 + begin : end]
            if not header_name.startswith("TMCompiler"):
                includes.add(header_name)

    return includes


def what_is_used(lines: List[str]) -> Set[str]:
    used: Set[str] = set()

    in_string: bool = False
    in_line_comment: bool = False
    in_block_comment: bool = False

    for line in lines:
        in_string = False
        in_line_comment = False
        curr_index = 0

        while curr_index < len(line):
            if in_block_comment:
                if (
                    curr_index + len("*/") <= len(line)
                    and line[curr_index : curr_index + len("*/")] == "*/"
                ):
                    in_block_comment = False
                    curr_index += len("*/")
                else:
                    curr_index += 1
                continue
            # in_block_comment False
            if in_line_comment:
                curr_index = len(line)
                continue
            # in_line_comment False
            if in_string:
                if line[curr_index] == '"':
                    in_string = False
                    curr_index += 1
                elif line[curr_index : curr_index + 1] == "\\":
                    curr_index += 2
                else:
                    curr_index += 1
                continue
            # in_string False
            if (
                curr_index + len("/*") <= len(line)
                and line[curr_index : curr_index + len("/*")] == "/*"
            ):
                in_block_comment = True
                curr_index += len("/*")
                continue
            if (
                curr_index + len("//") <= len(line)
                and line[curr_index : curr_index + len("//")] == "//"
            ):
                in_line_comment = True
                curr_index += len("//")
                continue
            if line[curr_index] == '"':
                in_string = True
                curr_index += 1
                continue
            # disregard the ones right after the #include
            if (
                curr_index + len("#include") <= len(line)
                and line[curr_index : curr_index + len("#include")] == "#include"
            ):
                curr_index = len(line)
                continue
            if (
                curr_index + len("std::") <= len(line)
                and line[curr_index : curr_index + len("std::")] == "std::"
            ):
                regex_pattern: str = "[^a-z_]"
                matches: List[Tuple[int, int]] = [
                    m.span()
                    for m in re.finditer(
                        regex_pattern, line[curr_index + len("std::") :]
                    )
                ]

                if matches:
                    end_std_index: int = curr_index + len("std::") + matches[0][0]
                    used.add(line[curr_index:end_std_index])
                    curr_index = end_std_index
                else:
                    curr_index += len("std::")

            curr_index += 1

    return used


def check_mismatches(
    included: List[str], used: List[str]
) -> Tuple[List[str], List[Tuple[str, str]], List[str]]:
    current_matchings: Dict[str, List[str]] = dict()

    for header in included:
        current_matchings[header] = []

    unrecognized_identifiers: List[str] = []
    unincluded_identifiers: List[Tuple[str, str]] = []
    unused_headers: List[str] = []

    for use in used:  # ex std::endl
        if use not in belongs_in_what_header:
            unrecognized_identifiers.append(use)
        elif (
            belongs_in_what_header[use] not in current_matchings
        ):  # if <iostream> not in included
            unincluded_identifiers.append((belongs_in_what_header[use], use))
        else:
            belonging_header: str = belongs_in_what_header[use]  # iostream
            current_matchings[belonging_header].append(use)

    for header in current_matchings:
        if len(current_matchings[header]) == 0:
            unused_headers.append(header)

    return unrecognized_identifiers, unincluded_identifiers, unused_headers


def check_std_includes_in_file(file: str) -> Optional[Panel]:
    lines: List[str] = []
    with open(file, "r") as f:
        lines = f.readlines()
        lines = [line.strip() for line in lines]

    included: List[str] = sorted(what_is_included(lines))
    used: List[str] = sorted(what_is_used(lines))

    unrecognized_identifiers: List[str]
    unincluded_identifiers: List[Tuple[str, str]]
    unused_headers: List[str]
    unrecognized_identifiers, unincluded_identifiers, unused_headers = check_mismatches(
        included, used
    )

    panel_lines: List[str] = []

    if len(unrecognized_identifiers) > 0:
        panel_lines.append(
            f"I do not recognize these identifiers: {unrecognized_identifiers}"
        )
    if len(unincluded_identifiers) > 0:
        panel_lines.append("You should have these includes:")
        for k, v in unincluded_identifiers:
            panel_lines.append(f"#include <{k}>    // {v}")
    if len(unused_headers) > 0:
        panel_lines.append(f"Unused headers: {sorted(unused_headers)}")

    if len(panel_lines) > 0:
        return Panel.fit(
            "\n".join(panel_lines), title=file, border_style="dark_orange3"
        )

    return None

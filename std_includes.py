import re
from typing import Dict, Generator, List, Set, Tuple

from rich import print
from rich.panel import Panel
from rich.progress import track

from check_files import c_plus_plus_files

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

            btwn: str = line[1 + begin : end]
            if not btwn.startswith("TMCompiler"):
                includes.add(btwn)

    return includes


def what_is_used(lines: List[str]) -> Set[str]:
    used: Set[str] = set()

    for line in lines:
        # disregard the ones right after the #include
        if line.startswith("#include"):
            continue

        # this includes comments in its search
        for index in find_all("std::", line):
            regex_pattern: str = "[^a-z_]"

            #  match = re.search(regex_pattern, line[index + 5:])
            matches: List[Tuple[int, int]] = [
                m.span() for m in re.finditer(regex_pattern, line[index + 5 :])
            ]

            if matches:
                end_std_index: int = index + len("std::") + matches[0][0]
                used.add(line[index:end_std_index])

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
            #  print(f"I do not know what header {use} belongs in")
            unrecognized_identifiers.append(use)
        elif (
            belongs_in_what_header[use] not in current_matchings
        ):  # if <iostream> not in included
            #  print(f"This file should include {belongs_in_what_header[use]} for {use}")
            unincluded_identifiers.append((belongs_in_what_header[use], use))
        else:
            belonging_header: str = belongs_in_what_header[use]  # iostream
            current_matchings[belonging_header].append(use)

    for header in current_matchings:
        if len(current_matchings[header]) == 0:
            unused_headers.append(header)

    return unrecognized_identifiers, unincluded_identifiers, unused_headers


def check_std_includes_in_file(file: str) -> None:
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
        print(
            Panel.fit("\n".join(panel_lines), title=file, border_style="dark_orange3")
        )
    else:
        print(f"{file} standard includes [yellow]OK")


def check_std_includes() -> None:
    files: List[str] = c_plus_plus_files()

    for file_name in track(files, description=" Checking standard includes..."):
        check_std_includes_in_file(file_name)


def main() -> None:
    check_std_includes()


if __name__ == "__main__":
    main()

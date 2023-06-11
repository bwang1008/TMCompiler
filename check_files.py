import glob
import subprocess

from rich import print
from rich.panel import Panel
from rich.progress import track

def c_plus_plus_files():
    cpp_files = glob.glob("TMCompiler/**/*.cpp", recursive=True)
    hpp_files = glob.glob("TMCompiler/**/*.hpp", recursive=True)

    return cpp_files + hpp_files

def get_file_contents(file_name):
    file_contents = ""
    with open(file_name, "r") as f:
        lines = f.readlines()
        file_contents = "".join(lines)

    return file_contents

def needs_to_run_clang_format(file_name):
    file_contents = get_file_contents(file_name)

    result = subprocess.run(["clang-format", "--style=file", file_name], capture_output=True)
    formatted_lines = result.stdout.decode("utf-8")

    return file_contents != formatted_lines


def needs_to_run_clang_tidy(file_name):
    file_contents = get_file_contents(file_name)

    result = subprocess.run(["clang-tidy", file_name, "-config=", "-header-filter=.*", "--quiet", "--", "-std=c++14", "-I."], capture_output=True)
    output = result.stdout.decode("utf-8")

    return len(output) != 0


def check_clang_format(files):
    needs_changing = []
    for file_name in track(files, description="Checking clang-format..."):
        if needs_to_run_clang_format(file_name):
            needs_changing.append(file_name)

    needs_changing.sort()

    print(Panel.fit('\n'.join(needs_changing), title="Please run clang-format on these files"))


def check_clang_tidy(files):
    needs_changing = []
    for file_name in track(files, description="Checking clang-tidy..."):
        if needs_to_run_clang_tidy(file_name):
            needs_changing.append(file_name)

    needs_changing.sort()

    print(Panel.fit('\n'.join(needs_changing), title="Please run clang-tidy on these files"))


def main():
    files = c_plus_plus_files()

    #  print(f"files = {files}")

    check_clang_format(files)
    check_clang_tidy(files)

if __name__ == "__main__":
    main()

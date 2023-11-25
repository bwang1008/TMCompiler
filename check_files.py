import datetime
import glob
import os
import subprocess

from rich import print
from rich.panel import Panel
from rich.progress import track

def c_plus_plus_files():
    cpp_files = glob.glob("TMCompiler/**/*.cpp", recursive=True)
    hpp_files = glob.glob("TMCompiler/**/*.hpp", recursive=True)

    exception_folders = [
            "TMCompiler/utils/vendor/**/*.cpp",
            "TMCompiler/utils/vendor/**/*.hpp",
    ]
    exception_files = set(file for file_glob in exception_folders for file in glob.glob(file_glob, recursive=True))

    return sorted(set(cpp_files + hpp_files) - exception_files)

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

    result = subprocess.run(["clang-tidy", file_name, "-config=", "-header-filter=.*", "--quiet", "--", "-std=c++17", "-I.", "-isystem", "TMCompiler/utils/vendor"], capture_output=True)
    output = result.stdout.decode("utf-8")

    return len(output) != 0


def check_clang_format(files):
    needs_changing = []
    for file_name in track(files, description="Checking clang-format..."):
        if needs_to_run_clang_format(file_name):
            needs_changing.append(file_name)

    needs_changing.sort()

    if len(needs_changing):
        print(Panel.fit('\n'.join(needs_changing), title=" Please run clang-format on these files", border_style='magenta'))
    else:
        print(Panel.fit('All  files formatted!', title='Status: 🎉', border_style='magenta'))


def check_clang_tidy(files):
    # read cache
    cache_lines = []
    cache_file_name = "cache_tidy.txt"

    try:
        with open(cache_file_name, "r") as f:
            cache_lines = f.readlines()
    except:
        pass

    previous_bad_files = set()
    past_run_time = datetime.datetime.min
    if len(cache_lines):
        # first line is timezone
        past_run_time = datetime.datetime.fromisoformat(cache_lines[0].strip())
        for i in range(1, len(cache_lines)):
            previous_bad_files.add(cache_lines[i].strip())

    # if clang-tidy file changed, always re-run
    clang_tidy_mtime = datetime.datetime.fromtimestamp(os.path.getmtime(".clang-tidy"))
    clang_tidy_changed = (clang_tidy_mtime > past_run_time)

    needs_changing = []
    for file_name in track(files, description="Checking clang-tidy...  "):

        file_mtime = datetime.datetime.fromtimestamp(os.path.getmtime(file_name))

        #  print(f"consider file {file_name}: has mtime {file_mtime}: is less than past run time? {file_mtime < past_run_time}")
        if not clang_tidy_changed and file_mtime < past_run_time:
            if file_name in previous_bad_files:
                needs_changing.append(file_name)
            continue

        if needs_to_run_clang_tidy(file_name):
            needs_changing.append(file_name)

    needs_changing.sort()

    # write to cache
    with open(cache_file_name, "w") as f:
        # write current timestamp
        f.write(datetime.datetime.now().isoformat())
        f.write("\n")

        for file_name in needs_changing:
            f.write(file_name)
            f.write("\n")


    if len(needs_changing):
        print(Panel.fit('\n'.join(needs_changing), title=" Please run clang-tidy on these files", border_style='red'))
    else:
        print(Panel.fit("All  files have been linted!", title='Status: 🎉', border_style='yellow'))


def main():
    files = c_plus_plus_files()

    #  print(f"files = {files}")

    check_clang_format(files)
    check_clang_tidy(files)

if __name__ == "__main__":
    main()

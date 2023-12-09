# Convert .lua file with in-code documentation into a .md file

import sys
import os
import re
import shutil

# %%

def find_py_and_copy_pyfile_to_md(input_lua, verbose=False):
    # get the dir name of the current lua file
    dir_name = os.path.dirname(input_lua)

    # Regular expression to match filenames with a .py extension
    pyfile_pattern = re.compile(r'\b[\w-]+\.py\b')
    # pyfile_pattern = re.compile(r'\(\.?[\w-]+\.py\)')

    # Initialize a set to keep track of unique Python file names
    unique_python_files = set()

    # Initialize a dictionary to store Python files and their content
    python_files = {}

    # Open the input file for reading
    with open(input_lua, "r") as file:
        lines = file.readlines()

    # Search for Python files and store their content
    for line_num, line in enumerate(lines, start=1):
        # Find filenames with .py extension in the line
        matches = pyfile_pattern.findall(line)
        if matches:
            for match in matches:
                # Ensure that each Python file is processed only once
                if match not in unique_python_files:
                    unique_python_files.add(match)
                    print(f"Line {line_num}: {match}")
                    match = dir_name + "/" + match
                    # if verbose:
                    #     print(f"Line {line_num}: {match}")
                    with open(match, "r") as py_source_file:
                        python_files[match] = py_source_file.read()

    # if verbose and len(python_files) > 0:
    #     print("In ", input_lua, ", these python_files were found\n",
    #           python_files.keys())

    # Create a Markdown file for each Python file
    for py_file, py_content in python_files.items():
        base_name = os.path.splitext(os.path.basename(py_file))[0]
        md_file_name = dir_name + "/" + base_name + ".md"
        if verbose:
            print("Markdown file {} created".format(md_file_name))
        with open(md_file_name, "w") as md_file:
            md_file.write(f"\page {base_name} {base_name}.py \n\n")
            md_file.write("```python\n")
            md_file.write(py_content)
            md_file.write("\n```\n")


# %%


def from_lua_to_md(lua_input_file, md_output_file, header, verbose=False):
    # Regular expression to match repeated "-" characters
    dash_pattern = re.compile(r"--+")
    # dash_pattern = re.compile(r"-1+")

    # Regular expression pattern to match the specified expression
    # pyfile_pattern = r'\b[\w-]+\.py\b'
    # pyfile_pattern = r'\(\.?([\w-]+\.py)\)'
    pyfile_pattern = re.compile(r"\b[\w-]+\.py\b")

    # Specify the input file, start marker, and end marker
    start_marker = "@doc"
    end_marker = "@end"
    # print(os.path.basename(filename))
    basename = os.path.splitext(lua_input_file)[0]
    if verbose:
        print("Working on file ", lua_input_file)

    # Initialize a flag to track whether we are inside the Markdown text
    inside_markdown = False

    # List to store the extracted lines
    extracted_lines = []

    # List to store the extracted lines with only the input
    extracted_lines_input_only = []
    extracted_lines_input_only.append("___\n")
    extracted_lines_input_only.append(
        "## The complete input is below:\n")
    extracted_lines_input_only.append(
        "You can copy/paste the text below or look in the file named ```{}```:\n".format(lua_input_file))
    extracted_lines_input_only.append("```\n")

    # Initialize counter
    counter = 0

    # Initialize a flag to know if a code block has been opened
    code_block = False

    # Open the file for reading
    with open(lua_input_file, "r") as file:
        for line in file:
            # Check if we are inside the Markdown text
            if inside_markdown:
                # Check if the current line contains the end marker
                if end_marker in line:
                    inside_markdown = False
                    # About to begin a Markdown code block
                    code_block = True
                    extracted_lines.append("```\n")
                else:
                    # remove the lua -- comment
                    clean_line = dash_pattern.sub("", line)
                    # add a doxygen ref for the python code
                    match = pyfile_pattern.search(clean_line)
                    if match:
                        # print(clean_line)
                        # Get the specific Python filename from the match
                        python_filename = match[0]
                        # print("python_filename=", python_filename)
                        base_name = os.path.splitext(python_filename)[0]
                        # Replace the matched pattern with the replacement text
                        replacement_text = " \subpage  " + base_name + " "
                        # print(replacement_text)
                        clean_line = clean_line.replace("("+python_filename+")",
                                                        replacement_text)
                        # print(clean_line)

                    extracted_lines.append(clean_line)
            # else, we are not inside the Markdown text
            else:
                # Check if we are about to enter a Markdown area
                if start_marker in line:
                    inside_markdown = True
                    if code_block:
                        # About to end a Markdown code block
                        code_block = False
                        extracted_lines.append("```\n")
                else:
                    # in the code snippets, do not add lines with ######
                    if "######" not in line:
                        extracted_lines.append(line)
                    extracted_lines_input_only.append(line)
            # increment counter
            counter += 1

    if code_block:
        # finish the md portion. if we just added ``` as the last action, then ingore that last line
        word_to_exclude = "```"
        if word_to_exclude not in extracted_lines[-1]:
            extracted_lines.append("```\n")
        else:
            extracted_lines = extracted_lines[:-1]
        # finish the input portion
        extracted_lines_input_only.append("```\n")

    # Print the extracted lines
    # if verbose:
    #     for line in extracted_lines:
    #         print(line, end="")
    #     print('\n\n####Done!!!')

    # Optionally, you can save the extracted lines to a new file
    with open(md_output_file, "w") as output_file:
        if len(header) > 0:
            output_file.write(header)
        output_file.writelines(extracted_lines)
        output_file.writelines(extracted_lines_input_only)
        output_file.writelines(
            "\nBack to \\ref lbs_tutorials")
        # "\nBack to [**Tutorial Home**](tutorials_transport.md)")


# %%


def load_jax_header(header_file):
    with open(header_file, 'r') as file:
        header = file.read()
    return header

# %%


def transform_all_lua_to_md(root_dir, jax_header, verbose=False):
    # Define the source and destination file extensions
    lua_extension = ".lua"

    # Walk through the directory and its subdirectories
    for root, dirs, files in os.walk(root_dir):
        for filename in files:
            # print(filename)
            if filename.endswith(lua_extension):
                # get the lua input file
                source_path = os.path.join(root, filename)
                # mkdir a transformed copy into an md file
                from_lua_to_md(source_path, jax_header, verbose)
                # check whether the lua file contains a link to a python file
                find_py_and_copy_pyfile_to_md(source_path, verbose)

# %%


def copymove_to_doc(source_dir, doc_dir, extension, move=False):

    # Check if the doc directory exists
    if not os.path.exists(doc_dir):
        # If it doesn't exist, create the directory
        os.makedirs(doc_dir)
        print(f"\nDirectory '{doc_dir}' created.\n")
    else:
        print(f"\nDirectory '{doc_dir}' already exists.\n")

    # Walk through the directory and its subdirectories
    for root, dirs, files in os.walk(source_dir):
        for filename in files:
            # print(filename)
            if filename.endswith(extension):
                # get the md file
                source_path = os.path.join(root, filename)
                relative_path = os.path.relpath(source_path, source_dir)
                # print("relative path =", relative_path)
                destination_path = os.path.join(doc_dir, relative_path)
                print(destination_path)
                # print(os.path.dirname(destination_path))

                # # Create the destination directory if it doesn't exist
                os.makedirs(os.path.dirname(destination_path), exist_ok=True)

                # destination_path = os.path.join(doc_dir, filename)
                # print(destination_path)

                if root == source_dir:
                    # continue  # Skip the root of the source directory
                    # Copy the file
                    shutil.copy(source_path, destination_path)
                else:
                    # Move the file
                    if move:
                        shutil.move(source_path, destination_path)
                    else:
                        shutil.copy(source_path, destination_path)

header = []
from_lua_to_md(sys.argv[1], sys.argv[2], header)

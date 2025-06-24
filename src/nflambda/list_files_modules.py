#!/usr/bin/env python3

import os
import re
from collections import defaultdict

# Define the root directory
root_dir = os.path.dirname(os.path.abspath(__file__))
dependency_file = os.path.join(root_dir, "00_dependency_management.md")

def is_module(directory):
    """Check if a directory is a module (contains CMakeLists.txt)"""
    return os.path.isfile(os.path.join(directory, "CMakeLists.txt"))

def find_files_and_modules():
    """Find all .c, .h files and modules in the core5g directory"""
    modules = []
    files = []
    
    for dirpath, dirnames, filenames in os.walk(root_dir):
        # Skip development_log directory
        if "development_log" in dirpath:
            continue
            
        rel_path = os.path.relpath(dirpath, root_dir)
        if rel_path != "." and is_module(dirpath):
            modules.append(rel_path)
        
        for file in filenames:
            if file.endswith((".c", ".h")) and "development_log" not in dirpath:
                file_path = os.path.join(rel_path, file)
                if file_path.startswith("."):
                    file_path = file_path[2:]  # Remove "./" prefix
                files.append(file_path)
    
    return sorted(modules), sorted(files)

def update_dependency_file(modules, files):
    """Update the dependency management markdown file"""
    if not os.path.exists(dependency_file):
        print(f"Error: {dependency_file} does not exist")
        return
    
    with open(dependency_file, 'r') as f:
        content = f.read()
    
    # Create the new content for the file and module list section
    modules_section = "### Modules\n"
    for module in modules:
        modules_section += f"- {module}\n"
    
    files_section = "\n### Files\n"
    
    # Group files by directory
    files_by_dir = defaultdict(list)
    for file_path in files:
        dir_name = os.path.dirname(file_path)
        if dir_name == "":
            dir_name = "root"
        files_by_dir[dir_name].append(os.path.basename(file_path))
    
    # Add files grouped by directory
    for dir_name in sorted(files_by_dir.keys()):
        if dir_name == "root":
            files_section += f"\n**Root Directory**\n"
        else:
            files_section += f"\n**{dir_name}/**\n"
            
        for file_name in sorted(files_by_dir[dir_name]):
            files_section += f"- {file_name}\n"
    
    new_content = modules_section + files_section
    
    # Replace the content between the markers
    pattern = re.compile(r'<!-- BEGIN_FILE_MODULE_LIST -->.*?<!-- END_FILE_MODULE_LIST -->', re.DOTALL)
    updated_content = pattern.sub(f'<!-- BEGIN_FILE_MODULE_LIST -->\n{new_content}\n<!-- END_FILE_MODULE_LIST -->', content)
    
    # Write the updated content back to the file
    with open(dependency_file, 'w') as f:
        f.write(updated_content)
    
    print(f"Successfully updated {dependency_file} with {len(modules)} modules and {len(files)} files")

if __name__ == "__main__":
    modules, files = find_files_and_modules()
    update_dependency_file(modules, files)

#!/usr/bin/env python2
# -*- coding: utf-8 -*-
from argparse import ArgumentParser
from re import compile, sub
from os import walk, path, chdir

license_comments_header_regex = compile(r"^(//[^\n]*\n)*\n*")
source_extensions = [".h", ".c", ".cpp"]
source_directories = ["rtuartscreader", "tests", "log"]

def add_license(path_to_file, license_text):
    with open(path_to_file, 'rb') as f:
        data = f.read()

    data = sub(license_comments_header_regex, license_text, data)

    with open(path_to_file, 'wb') as f:
        f.write(data);

def read_file(source):
    with open(source, 'r') as f:
        return f.read()

def license_files(directory, license_text):
    for dirpath, dnames, fnames in walk(directory):
        for f in fnames:
            if any(map(lambda extension: f.endswith(extension), source_extensions)):
                add_license(path.join(dirpath, f), license_text)

parser = ArgumentParser(description='Put license header to the source files')

args = parser.parse_args()

execute_path = path.dirname(path.realpath(__file__))
project_root = path.dirname(execute_path)
chdir(project_root)

license_text_path = path.join(project_root, "scripts", "data", "license_header.h.in")
license_text = read_file(license_text_path)

for source_directory in source_directories:
    license_files(source_directory, license_text)

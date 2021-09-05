from conans import ConanFile, tools, CMake
from conans.util.files import mkdir, walk
import os
import subprocess
import sys
import shutil
import fnmatch
import platform

class KrienGraphiXConan(ConanFile):
    name = "KrienGraphiX"
    version = "2.0.0"
    url = "None"
    license = "None"
    settings = "os", "compiler", "build_type", "arch"
    description = "<Description of KrienGraphiX here>"
    generators = "cmake","cmake_multi"
    #exports_sources = "src/*"
    options = {}
    default_options = {}

    def requirements(self):
        self.requires("boost/1.75.0")
        self.requires("gtest/1.11.0")

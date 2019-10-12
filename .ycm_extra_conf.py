# from distutils.sysconfig import get_python_inc
import platform
import os
import ycm_core
import copy

DEBUG = 0
c_language = 0

if platform.system() == "Windows":
    ycm_compilation_log_file = os.path.join(os.getenv("userprofile"),
                                            "ycm_compilation.log")
else:
    ycm_compilation_log_file = os.path.join(os.getenv("HOME"),
                                            "ycm_compilation.log")


def append_to_ycm_compilation_log_file(args):
    with open(ycm_compilation_log_file,
              mode="at") as append_file:
        for k, v in args.items():
            append_file.write(
                "{0:<25s}:  {1:<s}\n".format(k.__str__(), v.__str__()))
            append_file.write("\n")


def debug_here():
    with open(ycm_compilation_log_file,
              mode="at") as append_file:
        append_file.write("HERE is reachable.\n")


DEBUG and append_to_ycm_compilation_log_file(
    {"Debug ycm_extra_conf.py": "Yes"})

common_clang_flags = [
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Wno-long-long',
    '-Wno-variadic-macros',
    '-fexceptions',
    '-DNDEBUG'
]

common_linux_header_folder = list(filter(os.path.exists,
                                         ["/usr/include",
                                          "/usr/local/include",
                                          "/usr/share/include"]))


# windows header files handler
mscv_aux_find_level1 = 5
mscv_aux_find_level2 = 5
windows_exclude_include_path = []
msvc_paths = []


def list_sub_dir(par):
    return list(filter(lambda x: os.path.isdir(os.path.join(par, x)),
                       os.listdir(par)))


stl_files = {"algorithm", "array", "bitset", "complex", "deque", "exception",
             "iostream", "istream", "ostream"}  # just short list


def path_include_stl(path):
    if not os.path.exists(path):
        return False
    path_holder = set(os.listdir(path))
    return path_holder.issuperset(stl_files)


def msvc_level1(par, level):
    DEBUG and append_to_ycm_compilation_log_file({
        "msvc list length": len(msvc_paths).__str__(),
        "function": "msvc_level1",
        "arg1": par.__str__(),
        "arg2": level.__str__()})
    if level >= mscv_aux_find_level1:
        return None
    for subdir in list_sub_dir(par):  # priority depth traversal
        if subdir.upper() == "MSVC":
            msvc_paths.append(os.path.join(par, subdir))
        else:
            msvc_level1(os.path.join(par, subdir), level + 1)


def msvc_level2(par, level):
    DEBUG and append_to_ycm_compilation_log_file({
        "function": "msvc_level2",
        "arg1": par.__str__(),
        "arg2": level.__str__()})
    if level >= mscv_aux_find_level2:
        return None
    for subdir in list_sub_dir(par):
        if subdir == "include":
            if path_include_stl(os.path.join(par, subdir)):
                return os.path.join(par, subdir)
            else:
                windows_exclude_include_path.append(os.path.join(par, subdir))
        else:
            temp_holder = msvc_level2(os.path.join(par, subdir), level + 1)
            if temp_holder:
                return temp_holder
    return None


def msvc_path():
    DEBUG and append_to_ycm_compilation_log_file({
        "function": "msvc_path"})
    if platform.system() != 'Windows':
        return None
    msvs_path = os.path.join(os.getenv("ProgramFiles(x86)"),
                             "Microsoft Visual Studio")
    if not os.path.exists(msvs_path):
        return None
    msvc_level1(msvs_path, 0)
    for msvc_dir in msvc_paths:
        msvs_vc_include = msvc_level2(msvc_dir, 0)
        if msvs_vc_include:
            return msvs_vc_include
    return None


windows_header_folder = [msvc_path()]


default_c_flags = common_clang_flags + [
    '-x',
    'c',
    '-std=gnu11'
]

default_cpp_flags = common_clang_flags + [
    '-x',
    'c++',
    '-std=gnu++14'
]

linux_c_header_folder = copy.copy(common_linux_header_folder)
linux_cpp_header_folder = copy.copy(common_linux_header_folder)
for folder in common_linux_header_folder:
    if os.path.exists(folder + "/c++"):
        if sum(list(map(lambda x: os.path.isfile(folder + "/c++/" + x),
                        os.listdir(folder + "/c++")))) > 0:
            linux_cpp_header_folder += [folder + "/c++"]
        else:
            for _final_cpp in os.listdir(folder + "/c++"):
                if os.path.islink(folder + "/c++/" + _final_cpp):
                    linux_cpp_header_folder += [folder + "/c++/" + _final_cpp]

if c_language:
    flags = default_c_flags
else:
    flags = default_cpp_flags

if platform.system() == "Windows":
    append_include_ = windows_header_folder
else:
    if c_language:
        append_include_ = linux_c_header_folder
    else:
        append_include_ = linux_cpp_header_folder

for _include_ in append_include_:
    if _include_:
        flags.append('-I')
        flags.append(_include_)

compilation_database_folder = ''

if os.path.exists(compilation_database_folder):
    database = ycm_core.CompilationDatabase(compilation_database_folder)
else:
    database = None

if c_language:
    source_extensions = ['.c']
else:  # cpp
    source_extensions = ['.c', '.cpp', '.cxx', '.hpp', '.cc']


def DirectoryOfThisScript():
    return os.path.dirname(os.path.abspath(__file__))


def IsHeaderFile(filename):
    extension = os.path.splitext(filename)[1]
    return extension in ['.h']


def GetCompilationInfoForFile(filename):
    if IsHeaderFile(filename):
        basename = os.path.splitext(filename)[0]
        for extension in source_extensions:
            replacement_file = basename + extension
            if os.path.exists(replacement_file):
                compilation_info = \
                    database.GetCompilationInfoForFile(replacement_file)
                if compilation_info.compiler_flags_:
                    return compilation_info
            return None
        return database.GetCompilationInfoForFile(filename)


if DEBUG:
    append_to_ycm_compilation_log_file(
        {"current_working_directory": os.getcwd(),
         "msvc_paths": msvc_paths.__str__(),
         "common_clang_flags": common_clang_flags,
         "common_linux_header_folder": common_linux_header_folder,
         "default_c_flags": default_c_flags,
         "default_cpp_flags": default_cpp_flags,
         "linux_c_header_folder": linux_c_header_folder,
         "linux_cpp_header_folder": linux_cpp_header_folder,
         "windows_header_folder": windows_header_folder})


def Settings(**kwargs):
    kwargs["flags"] = flags
    append_to_ycm_compilation_log_file(kwargs)
    if not database:
        return {
            'flags': flags,
            'include_paths_relative_to_dir': DirectoryOfThisScript()
        }

    compilation_info = GetCompilationInfoForFile(kwargs["filename"])
    if not compilation_info:
        return None

    final_flags = list(compilation_info.compiler_flags_)

    return {
        'flags': final_flags,
        'include_paths_relative_to_dir': compilation_info.compiler_working_dir_
    }

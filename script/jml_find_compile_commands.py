import os

def __compile_commands_has_database(directory):
    pass
    possible_db = os.path.join(directory, 'compile_commands.json')
    if os.path.exists(possible_db):
        if os.access(possible_db, os.R_OK):
            return True
    return False

def __compile_commands_search_directories(root_dir):
    build_dir = os.path.join(root_dir, 'build')

    parent_dir = os.path.join(root_dir, '..')
    parent_build_dir = os.path.join(parent_dir, 'build')

    return (root_dir, build_dir, parent_dir, parent_build_dir)


def PossibleCompilationDatabase(root_dir):
    search_dirs = __compile_commands_search_directories(root_dir)
    for directory in search_dirs:
        if __compile_commands_has_database(directory):
            return directory
    return ''

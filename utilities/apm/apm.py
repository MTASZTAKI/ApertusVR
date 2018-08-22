from string import Template
import argparse
import os
import sys
from shutil import copytree, ignore_patterns, rmtree

uncapitalize = lambda s: s[:1].lower() + s[1:] if s else ''
capitalize = lambda s: s[:1].upper() + s[1:] if s else ''

def query_yes_no(question, default="yes"):
    """Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is True for "yes" or False for "no".
    """
    valid = {"yes": True, "y": True, "ye": True,
             "no": False, "n": False}
    if default is None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "
                             "(or 'y' or 'n').\n")

class ApePackageManager:
    variable = "blah"

    def __init__(self):
        self.templatesPath = './templates/'
        self.samplesCMakeFile = '../../samples/CMakeLists.txt'
        self.pluginsCMakeFile = '../../plugins/CMakeLists.txt'

    def createPluginTemplate(self, args):
        pluginName = uncapitalize(args.name)
        pluginPath = '../../plugins/' + pluginName + '/'
        pluginClassName = 'Ape' + capitalize(pluginName) + 'Plugin'
        pluginHeaderName = 'APE_' + pluginClassName.upper() + 'PLUGIN_H'
        pluginsCMakeListsOptionName = 'APE_PLUGIN_' + capitalize(pluginName)
        pluginsCMakeListsCaptionName = capitalize(pluginName) + 'Plugin'

        # create plugin folder
        if not os.path.exists(pluginPath):
            os.makedirs(pluginPath)

        # open plugin header template file
        pluginHeaderFileName = pluginClassName + '.h'
        pluginHeaderFilePath = pluginPath + pluginHeaderFileName
        with open(self.templatesPath + 'plugin_h.template', 'r') as pluginHeaderTemplateFile:
            data = pluginHeaderTemplateFile.read()

            # replace variables in header template
            s = Template(data)
            res = s.substitute(pluginHeaderName=pluginHeaderName, pluginClassName=pluginClassName)
            # print(res)

            # write plugin header file
            with open(pluginHeaderFilePath, "w") as pluginHeaderFile:
                pluginHeaderFile.write(res)

        # open plugin cpp template file
        pluginCppFileName = pluginClassName + '.cpp'
        pluginCppFilePath = pluginPath + pluginCppFileName
        with open(self.templatesPath + 'plugin_cpp.template', 'r') as pluginCppTemplateFile:
            data = pluginCppTemplateFile.read()

            # replace variables in cpp template
            s = Template(data)
            res = s.substitute(pluginClassName=pluginClassName)
            # print(res)

            # write plugin cpp file
            with open(pluginCppFilePath, "w") as pluginCppFile:
                pluginCppFile.write(res)

        # open plugin cmake template file
        pluginCmakeFilePath = pluginPath + 'CMakeLists.txt'
        with open(self.templatesPath + 'plugin_cmake.template', 'r') as pluginCmakeTemplateFile:
            data = pluginCmakeTemplateFile.read()

            # replace variables in cmake template
            s = Template(data)
            res = s.substitute(pluginClassName=pluginClassName, pluginHeaderFileName=pluginHeaderFileName, pluginCppFileName=pluginCppFileName)
            # print(res)

            # write plugin cmake file
            with open(pluginCmakeFilePath, "w") as pluginCmakeFile:
                pluginCmakeFile.write(res)

        print(pluginClassName + ' has been created.')

        if query_yes_no("Add to plugins/CMakeLists.txt?"):
            # open pluginsCMakeLists.txt.template file
            with open(self.templatesPath + 'pluginsCMakeLists.txt.template', 'r') as pluginsCMakeListsTemplateFile:
                data = pluginsCMakeListsTemplateFile.read()

                # replace variables in pluginsCMakeLists.txt.template string
                s = Template(data)
                res = s.substitute(pluginOptionName=pluginsCMakeListsOptionName, pluginName=pluginName, pluginCaptionName=pluginsCMakeListsCaptionName)

                # write plugins CMakeLists.txt file
                with open(self.pluginsCMakeFile, "a") as pluginsCmakeFile:
                    pluginsCmakeFile.write(res)

    def createSampleTemplate(self, args):
        sampleName = uncapitalize(args.name)
        samplePath = '../../samples/' + sampleName + '/'
        configPath = samplePath + 'configs/'
        sampleClassName = 'Ape' + capitalize(sampleName)
        samplesCMakeListsOptionName = 'APE_SAMPLES_' + capitalize(sampleName)

        # create sample folder
        if not os.path.exists(samplePath):
            os.makedirs(samplePath)

        # open sample cpp template file
        sampleCppFileName = sampleClassName + '.cpp'
        sampleCppFilePath = samplePath + sampleCppFileName
        with open(self.templatesPath + 'sample_cpp.template', 'r') as sampleCppTemplateFile:
            data = sampleCppTemplateFile.read()

            # replace variables in sample cpp template
            s = Template(data)
            res = s.substitute(sampleName=sampleName)

            # write sample cpp file
            with open(sampleCppFilePath, "w") as sampleCppFile:
                sampleCppFile.write(res)

        if os.path.exists(configPath):
            rmtree(configPath)
        copytree(self.templatesPath + 'configs/', configPath, ignore=ignore_patterns('*.template'))

        # open sample config cmake template file
        with open(self.templatesPath + 'configs/local_monitor/CMakeLists.txt.template', 'r') as sampleDefaultConfigCmakeTemplateFile:
            data = sampleDefaultConfigCmakeTemplateFile.read()

            # replace variables in sample config cmake template
            s = Template(data)
            res = s.substitute(sampleName=sampleName)

            # write sample config cmake file
            with open(configPath + 'local_monitor/' + 'CMakeLists.txt', "w") as sampleDefaultConfigCmakeFile:
                sampleDefaultConfigCmakeFile.write(res)

        # open sample cmake template file
        sampleCmakeFilePath = samplePath + 'CMakeLists.txt'
        with open(self.templatesPath + 'sample_cmake.template', 'r') as sampleCmakeTemplateFile:
            data = sampleCmakeTemplateFile.read()

            # replace variables in sample cmake template
            s = Template(data)
            res = s.substitute(sampleName=sampleName, sampleClassName=sampleClassName, sampleCppFileName=sampleCppFileName)
            # print(res)

            # write sample cmake file
            with open(sampleCmakeFilePath, "w") as sampleCmakeFile:
                sampleCmakeFile.write(res)

        print(sampleClassName + ' has been created.')

        if query_yes_no("Add to samples/CMakeLists.txt?"):
            # open samplesCMakeLists.txt.template file
            with open(self.templatesPath + 'samplesCMakeLists.txt.template', 'r') as samplesCMakeListsTemplateFile:
                data = samplesCMakeListsTemplateFile.read()

                # replace variables in samplesCMakeLists.txt.template string
                s = Template(data)
                res = s.substitute(sampleOptionName=samplesCMakeListsOptionName, sampleName=sampleName)

                # write samples CMakeLists.txt file
                with open(self.samplesCMakeFile, "a") as samplesCmakeFile:
                    samplesCmakeFile.write(res)

apm = ApePackageManager()

parser = argparse.ArgumentParser(description='Optional app description')
subparsers = parser.add_subparsers(help='sub-command help')

# create the parser for 'status' command
parser_status = subparsers.add_parser('status', help='a help')
parser_status.add_argument('bar', type=int, help='bar help')

# create the parser for 'create' command
parser_create = subparsers.add_parser('create', help='Creates a new object (plugin or sample)')
parser_create_subparsers = parser_create.add_subparsers(help='sub-command help')

# parse plugin arguments
parser_create_plugin = parser_create_subparsers.add_parser('plugin', help='Creates a new plugin')
parser_create_plugin.add_argument('name', type=str, default='testPlugin', help='The name of the plugin to be created')
parser_create_plugin.set_defaults(func=apm.createPluginTemplate)

# parse plugin arguments
parser_create_sample = parser_create_subparsers.add_parser('sample', help='Creates a new sample')
parser_create_sample.add_argument('name', type=str, default='testSample', help='The name of the sample to be created')
parser_create_sample.set_defaults(func=apm.createSampleTemplate)

args = parser.parse_args()

print("Argument values:")
print(args)
print(args.name)

args.func(args)

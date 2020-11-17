import os
import re
import setuptools
import subprocess

def get_clhep_info():
    """
    Relies on the `clhep-config` utility present in system. User may submit
    custom `clhep-config` executable using `CLHEP_CONFIG` environment variable.
    Optionally, the compilation assets may be overriden with environment
    variables:
        CLHEP_INCLUDE_DIRS -- colon-separated list of include directories
        CLHEP_LIBDIRS -- space separated listlibraries to link
        CLHEP_LIB_DIRS -- colon-separated list of library directories
    """
    res = {}
    clhepExec = os.getenv('CLHEP_CONFIG', 'clhep-config')
    # get include dir
    if not os.getenv('CLHEP_INCLUDE_DIRS'):
        r = subprocess.run([clhepExec, '--include'], stdout=subprocess.PIPE)
        res['include'] = re.findall(r'-I\s?\"?([^\s\"]+)\"?', r.stdout.decode())
    else:
        res['include'] = os.getenv('CLHEP_INCLUDE_DIRS').split(':')
    # get libraries to link
    if not os.getenv('CLHEP_LIBRARIES'):
        r = subprocess.run([clhepExec, '--libs'], stdout=subprocess.PIPE)
        res['libs'] = re.findall(r'-l\s?\"?([^\s\"]+)\"?', r.stdout.decode())
    else:
        res['libs'] = os.getenv('CLHEP_LIBRARIES').split()
    if not os.getenv('CLHEP_LIB_DIRS'):
        r = subprocess.run([clhepExec, '--libs'], stdout=subprocess.PIPE)
        res['lib_dirs'] = re.findall(r'-L\s?\"?([^\s\"]+)\"?', r.stdout.decode())
    else:
        res['lib_dirs'] = os.getenv('CLHEP_LIB_DIRS').split(':')
    return res

gCAssets = get_clhep_info()

evaluatorExtension = setuptools.Extension( "CLHEPEvaluator"
                , sources = ['CLHEPPyEval.cc']
                #, define_macros = [ ('MAJOR_VERSION', '0')
                #                  , ('MINOR_VERSION', '1')
                #                  ]
                # TODO: ^^^ $ clhep-config --version
                , include_dirs = gCAssets['include']
                , libraries = gCAssets['libs']
                , library_dirs = gCAssets['lib_dirs']
                , runtime_library_dirs = gCAssets['lib_dirs']
                )

setuptools.setup( name = 'CLHEPEvaluator'
                , version = '0.0.1'
                , description = 'A Python wrapper for native CLHEP evaluator'
                , author = 'Renat R. Dusaev'
                , author_email = 'renat.dusaev@cern.ch'
                , license = 'MIT'
                , long_description = '''
TODO: long description here
'''
                , ext_modules = [evaluatorExtension]
                )


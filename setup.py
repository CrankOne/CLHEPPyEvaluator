import os, setuptools

gClhepCfgExec = os.getenv('CLHEP_CONFIG', 'clhep-config')

evaluatorExtension = setuptools.Extension( "CLHEPEvaluator"
                , sources = ['CLHEPPyEval.cc']
                #, define_macros = [ ('MAJOR_VERSION', '0')
                #                  , ('MINOR_VERSION', '1')
                #                  ]
                # TODO: ^^^ $ clhep-config --version
                , include_dirs = ['/usr/include']  # TODO: $ clhep-config --include
                , libraries = ['CLHEP-2.4.1.3']  # TODO: $clhep-config --libs
                #, library_dirs = [os.path.abspath('../lib')]
                #, runtime_library_dirs = [os.path.abspath('../lib')]
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


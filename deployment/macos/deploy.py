import bundle
import deps
import sys
bundle.create_bundle('DNG')
deps.make_standalone('../../stage', 'standalone', True, 
                     '--no-rpath' in sys.argv, 
                     macports_workaround='--macports-workaround' in sys.argv)

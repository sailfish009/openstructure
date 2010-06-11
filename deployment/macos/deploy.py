import bundle
import deps
bundle.create_bundle('DNG')
deps.make_standalone('../../stage', 'standalone', True)

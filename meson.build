project('tbd', 'c',
    license : 'MIT',
    version: '0.1.0',
    meson_version : '>=0.55.0',
    default_options : [
        'c_std=c99',
        'werror=false',
        'warning_level=3'
    ]
)

subdir('src')

includes = include_directories('include')

executable('tbdc', srcs, include_directories : includes)

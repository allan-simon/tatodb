from distutils.core import setup, Extension

tatodb_module = Extension(
    'tatodb',
    define_macros = [
        ('MAJOR_VERSION', '1'),
        ('MINOR_VERSION', '0')
    ],
    include_dirs = ['/usr/local/include'],
	libraries = ['tato'],
    library_dirs = [
        '/usr/local/lib',
        '/usr/lib'
    ],
    sources = [
        'tato_item.c',
        'tato_module.c',
        'tato_db.c',
        'tato_relation.c',
        'tato_meta.c',
    ]
)

setup (
    name = 'tatodb',
    version = '0.1',
    description = 'Python binding for tatodb',
    author = 'Allan  SIMON',
    author_email = 'allan.simon@supinfo.com',
    url = '',
    long_description = '''Python binding for tatodb.''',
    ext_modules = [tatodb_module]
)

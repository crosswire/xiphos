
def okmsg(kw):
    """
    some customization for displaying pkg-config values on the line with ok msg
    to be able use this function there is need for file ./wafadmin/Tools/config_c.py
    from svn in r5753
    """
    return 'ok %s' % kw['success'].strip()


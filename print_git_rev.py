# encoding: utf-8

# usage: 
#    please, define the following ENVIRONMENT VARIABLES to 
#    configure this script:
#
#    FLAG = identify which compilation FLAGs should be used 
#      ... RESETALL - to reset all configuration
#      ... NO_AUTOREGISTRY - to remove auto registration for each device
#
#    KONKER_USER=<username>
#      ... define the device's username on the platform 
#    KONKER_PASSWD=<pwd> 
#      ... define device's password on the platform
#    WS<#>=<SSID> WP<#>=<PASSWORD>
#      ... where <#> is the # ... 1, 2, 3, 4 ... of the network
#      ... <SSID> name of the network
#      ... <PASSWORD> used to connect to the correspondent SSID
#
#

import os.path
import subprocess

def _minimal_ext_cmd(cmd):
    # construct minimal environment
    env = {}
    for k in ['SYSTEMROOT', 'PATH']:
        v = os.environ.get(k)
        if v is not None:
            env[k] = v
    # LANGUAGE is used on win32
    env['LANGUAGE'] = 'C'
    env['LANG'] = 'C'
    env['LC_ALL'] = 'C'
    out = subprocess.Popen(cmd, stdout=subprocess.PIPE, env=env).communicate()[0]
    return out

def get_git_hash():
    '''
    Gets the last GIT commit hash and date for the repository, using the
    path to this file.
    '''
    try:
        out = _minimal_ext_cmd(['git', 'rev-parse', '--short', 'HEAD'])
        GIT_REVISION = out.strip().decode('ascii')
    except:
        GIT_REVISION = None

    return GIT_REVISION

def setFlag(flag):
    v = os.environ.get(flag) or None
    if (v):
        return '-D_{}=\'"{}"\' '.format(flag, v)
    return ''


if __name__ == '__main__':
    flag = ''
    flag = '{}{}'.format(flag, setFlag('WS1'))
    flag = '{}{}'.format(flag, setFlag('WP1'))
    flag = '{}{}'.format(flag, setFlag('WS2'))
    flag = '{}{}'.format(flag, setFlag('WP2'))
    flag = '{}{}'.format(flag, setFlag('WS3'))
    flag = '{}{}'.format(flag, setFlag('WP3'))
    flag = '{}{}'.format(flag, setFlag('KONKER_USER'))
    flag = '{}{}'.format(flag, setFlag('KONKER_PASSWD'))
    flag = '{}{}'.format(flag, setFlag('TEST_WIFI_SSID'))
    flag = '{}{}'.format(flag, setFlag('TEST_WIFI_PASSWD'))
    flag = '{}{}'.format(flag, setFlag('DATA_PIN'))

    if os.environ.get('FLAG'):
        for v in os.environ.get('FLAG').split(','):
            flag='{} -D{}'.format(flag, v.strip())
    
    print('-g3 -DPIO_SRC_REV=\'"%s"\' %s' % (get_git_hash(), flag))

    f = open('flags.txt', 'w+')
    f.write(flag)
    f.flush()
    f.close()

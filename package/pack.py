#!/usr/bin/python3
# -*- coding: utf-8 -*-

import datetime
import glob
import os
import tarfile
from os.path import abspath, basename, dirname, join, relpath
import sys

import tqdm

CWD = abspath(dirname(__file__))

UPGRADE_MAPPINGS = {}
INSTALLER_MAPPINGS = {}


def safe_path(path):
    return join(CWD, path)


def create_mappings(dname, mappings=None):
    if mappings is None:
        mappings = {}
    else:
        mappings = mappings.copy()

    dname = join(CWD, dname)
    for root, _, fnames in os.walk(dname):
        for fname in fnames:
            fname = join(root, fname)
            mappings[join('.', relpath(fname, dname))] = fname

    return mappings


def tar(client_name):
    global UPGRADE_MAPPINGS

    version = datetime.datetime.now().strftime('%Y.%m.%d.%H%M%S')
    tgz_fname = u'【{}】quface-app-upgrade-{}.tgz'.format(
        basename(client_name), version)

    print('packaging {} ...'.format(tgz_fname))
    package = tarfile.open(tgz_fname, 'w:gz')

    mappings = create_mappings(
        join(client_name, 'upgrade'), UPGRADE_MAPPINGS)
    for arcname in tqdm.tqdm(mappings):
        package.add(
            mappings[arcname], arcname, recursive=False)
    package.close()

    tgz_fname = u'【{}】quface-app-installer-{}.tgz'.format(
        basename(client_name), version)
    print('packaging {} ...'.format(tgz_fname))

    package = tarfile.open(tgz_fname, 'w:gz')

    mappings = create_mappings(
        join(client_name, 'upgrade'), INSTALLER_MAPPINGS)
    mappings = create_mappings(
        join(client_name, 'installer-extra'), mappings)

    for arcname in tqdm.tqdm(mappings):
        package.add(
            mappings[arcname], arcname, recursive=False)
    package.close()


if __name__ == "__main__":
    UPGRADE_MAPPINGS = create_mappings(
        'common/upgrade', UPGRADE_MAPPINGS)
    INSTALLER_MAPPINGS = create_mappings(
        'common/installer-extra', UPGRADE_MAPPINGS)

    if len(sys.argv) > 1:
        client_name = safe_path(sys.argv[1])
        if client_name[-1] == '/':
            client_name = client_name[:-1]
        tar(client_name)
    else:
        for client_name in glob.glob(safe_path('by-client/*')):
            tar(client_name)

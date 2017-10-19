#!/bin/bash

set -ve
distro="${1}"
tag="${2}"
version="${distro}:${tag}"
srcdir="$(readlink -f "$(dirname "${0}")/../../")"
container=xiphos_test
# First, create the container
docker pull "${version}"
# Install necessary packages
docker run --name "${container}" --volume "${srcdir}:/xiphos" -t -d "${version}" /bin/bash
case "${distro}" in
    fedora|centos)
        if [ "${distro}" == "centos" ]; then
            mgr=yum
            docker exec -it "${container}" yum install -y epel-release
        else
            mgr=dnf
        fi
        installer="${mgr} install -y sword-devel \
                    gcc-c++ \
                    gtk3-devel \
                    biblesync-devel \
                    dbus-glib-devel \
                    docbook-utils \
                    GConf2-devel \
                    gettext \
                    libglade2-devel \
                    gnome-doc-utils \
                    intltool \
                    libgsf-devel \
                    libuuid-devel \
                    rarian-compat \
                    gtkhtml3-devel"
        if [[ "${tag}" == "7" || "${tag}" == "25" ]]; then
            installer="${installer} webkitgtk3-devel"
        else
            installer="${installer} webkitgtk4-devel"
        fi
        ;;
    ubuntu|debian)
        docker exec -t "${container}" apt-get update
        installer="apt-get install -y
                   libsword-dev \
                   gcc \
                   g++ \
                   libgtk-3-dev \
                   libdbus-glib-1-dev \
                   docbook-utils \
                   libgconf2-dev \
                   gettext \
                   libglade2-dev \
                   gnome-doc-utils \
                   intltool \
                   libgsf-1-dev \
                   uuid-dev \
                   rarian-compat \
                   libwebkitgtk-3.0-dev \
                   libbiblesync-dev"
        ;;
esac
docker exec -t "${container}" ${installer}

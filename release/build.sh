#!/bin/bash
set -e -o pipefail

script="$(readlink -f "${0}")"
srcdir="$(dirname "$(dirname "${script}")")"

if [ -z "${1}" ]; then
	echo "Specify docker image, e.g.: ${0} fedora:latest"
	exit 1
fi
image="${1}"

if [ -z "${2}" ]; then
	target="./release/linux.sh"
	echo "No script specified, running ${target}"
else
	target="./${2}"
	echo "Running ${target}"
fi

docker run --rm -v "${srcdir}:/workspace" -t "${image}" /bin/bash -c "cd /workspace && ${target}"

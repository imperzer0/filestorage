#!/bin/bash
# Maintained by imper <imperator999mcpe@gmail.com>

pkgname=filestorage
pkgver=17.2
pkgrel=1
pkgdesc='File storage service server for linux'
author="imperzer0"
arch=('any')
url=https://github.com/$author/$pkgname
license=('GPL3')
depends=('openssl' 'gcc' 'mariadb' 'mariadb-connector-cpp-git' 'lua' 'libzip')
makedepends=('cmake' 'git' 'gcc' 'make' 'mariadb-connector-cpp-git' 'lua' 'libzip')

_srcprefix="local:/"
_libfiles=("CMakeLists.txt" "main.cpp"
	"server.cpp" "server.h"
	"database.cpp" "database.h"
	"constants.hpp" "templates.hpp"
	"sha256.cpp" "sha256.hpp"
	"config_script.cpp" "config_script.h"
	"lua_config_libfunctions.cpp" "lua_config_libfunctions.h"
	"Gen_QR.cpp" "Gen_QR.h"
	"zip_dir.cpp" "zip_dir.h"
)

_rcfiles=(
	"resources/config_lib.lua"
	"resources/error.html"
	"resources/explorer.html"
	"resources/favicon.ico"
	"resources/filestorageconf.lua"
	"resources/index.html"
	"resources/invalid_credentials.html"
	"resources/uploader.html"
)

# shellcheck disable=SC2068
for _libfile in ${_libfiles[@]}; do
	source=(${source[@]} "$_srcprefix/$_libfile")
	sha256sums=(${sha256sums[@]} "SKIP")
done

# shellcheck disable=SC2068
for _rcfile in ${_rcfiles[@]}; do
	source=(${source[@]} "$_srcprefix/$_rcfile")
	sha256sums=(${sha256sums[@]} "SKIP")
done

external=(${external[@]} "https://raw.githubusercontent.com/cesanta/mongoose/d5993ba27ece4b406c230eca63b76dd5d2c28a2d/mongoose.c")
sha256sums=(${sha256sums[@]} "a0f97a894547cf98cbfc9b276fc924288658c772cdfe3e6a8d60df91d9cc27f7")

external=(${external[@]} "https://raw.githubusercontent.com/cesanta/mongoose/d5993ba27ece4b406c230eca63b76dd5d2c28a2d/mongoose.h")
sha256sums=(${sha256sums[@]} "a486f3d5baa0d31c576b24f909ede017331f5b104dc3eac945aceafe40f8fe67")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/strscan/2f263154679e67ed44aa7fc4ae65829547e8290b/strscan.c")
sha256sums=(${sha256sums[@]} "ec146de96056e2a058bf3d965d93eb3be1c84fc971d572e7c95a98cb6dff9907")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/strscan/2f263154679e67ed44aa7fc4ae65829547e8290b/strscan.h")
sha256sums=(${sha256sums[@]} "50e97478733949f8a01c4d5178919c5c51a33df0d22c10762339570ba985bf9e")

external=(${external[@]} "https://raw.githubusercontent.com/nayuki/QR-Code-generator/720f62bddb7226106071d4728c292cb1df519ceb/cpp/qrcodegen.cpp")
sha256sums=(${sha256sums[@]} "1f3b3fcdac6954c32cf583ccd02ec9b5901f756a38c461acedc70be4a77d3757")

external=(${external[@]} "https://raw.githubusercontent.com/nayuki/QR-Code-generator/720f62bddb7226106071d4728c292cb1df519ceb/cpp/qrcodegen.hpp")
sha256sums=(${sha256sums[@]} "b779c3b156cf7a57ce789d6fee4fc991ccc2913774d26c909d22bb8f26b2a793")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/perceptron/8ee566f52cc90b261e4910acb69563a842642872/layer.cpp")
sha256sums=(${sha256sums[@]} "cb3be1ccd1f57776b28efd8ad2f86f09e98900cfd28f8d7d1dee738d5a4284ea")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/perceptron/8ee566f52cc90b261e4910acb69563a842642872/layer.hpp")
sha256sums=(${sha256sums[@]} "d1c06305cb07105411b80c80cff001d035b280a6cbe399b79195fd79d18419e0")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/perceptron/8ee566f52cc90b261e4910acb69563a842642872/matrix.cpp")
sha256sums=(${sha256sums[@]} "6c141e4648e0b2658bbd75bd69105833f81649453f8b61b210e93b47866e036a")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/perceptron/8ee566f52cc90b261e4910acb69563a842642872/matrix.hpp")
sha256sums=(${sha256sums[@]} "057b5609e5adc2dd014b39a088d31e25207988113b554c7b3b9c36aaa6aa2ac8")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/perceptron/8ee566f52cc90b261e4910acb69563a842642872/perceptron.cpp")
sha256sums=(${sha256sums[@]} "8f9f853c744ac59c0783f908485da8f5f8edda50cb15536185835634b991912f")

external=(${external[@]} "https://raw.githubusercontent.com/imperzer0/perceptron/8ee566f52cc90b261e4910acb69563a842642872/perceptron.hpp")
sha256sums=(${sha256sums[@]} "a609060ad581737923379f249aefab3fcfd87b7709a75f543851fc7b3a19d9af")

source=(${source[@]} ${external[@]})

notarch_prepare() {
	# shellcheck disable=SC2068
	for ex in ${external[@]}; do
		wget "$ex"
	done
}

_package_version=" ("$pkgver"-"$pkgrel")"

build() {
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ \
		-DPACKAGE_VERSION="$_package_version" -DAPPNAME="$pkgname" .
	make -j 6
}

package() {
	install -Dm755 $pkgname "$pkgdir/usr/bin/$pkgname"
}

notarch_package() {
	cp -f $pkgname "$pkgdir/usr/bin/$pkgname"
	chmod 755 "$pkgdir/usr/bin/$pkgname"
}

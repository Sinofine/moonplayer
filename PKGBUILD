# maintainer: hzy068808 at gmail.com
# contributer: a304yuanyuan at gmail.com
# contributer: rob.til.freedman@googlemail.com

pkgname=moonplayer-bili
pkgver=3.8
pkgrel=1
pkgdesc="A qt font-end for mplayer with the abilities of watching and downloading videos from chinese network"
arch=('i686' 'x86_64')
url="https://github.com/coslyk/moonplayer"
license=('GPL')
conflicts=('moonplayer')
depends=('python' 'qt5-x11extras' 'qt5-base' 'qt5-declarative' 'mpv')
makedepends=('cmake' 'git' 'qt5-tools')
source=(
	git+https://github.com/sinofine/moonplayer.git#branch=develop
	)
sha1sums=('SKIP')

prepare() {
  cd $srcdir/moonplayer
  git submodule update --init --recursive
}

build() {
	cd $srcdir/moonplayer
  mkdir build
  cd build

  cmake .. -DCMAKE_INSTALL_PREFIX=/usr

	make
}

package() {
	cd $srcdir/moonplayer/build

	make DESTDIR="${pkgdir}" install

	cd $pkgdir/usr/share

	mv icons pixmaps
}

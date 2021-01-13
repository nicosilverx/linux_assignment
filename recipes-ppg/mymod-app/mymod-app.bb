DESCRIPTION = "Mymod-app Linux Kernel Module"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://mymod-app.c"

S = "${WORKDIR}"

do_compile() {
	set CFLAGS -g 
	${CC} ${CFLAGS} -lm -pthread mymod-app.c ${LDFLAGS} -o mymod-app
	unset CFLAGS
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 mymod-app ${D}${bindir}
}

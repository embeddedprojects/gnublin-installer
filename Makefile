VERSION = 1.4.0.2
ARCHITECTURE = amd64
#ARCHITECTURE = i386
CPP = g++
-I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__
CXXFLAGS = -Wall -g `wx-config --cxxflags` -pthread -DVERSION='"$(VERSION)"'
# -lwx_gtk2u_richtext-2.8 -lwx_gtk2u_aui-2.8 -lwx_gtk2u_xrc-2.8 -lwx_gtk2u_qa-2.8 -lwx_gtk2u_html-2.8 -lwx_gtk2u_adv-2.8 -lwx_gtk2u_core-2.8 -lwx_baseu_xml-2.8 -lwx_baseu_net-2.8 -lwx_baseu-2.8
LDFLAGS = -lparted -lcurl -pthread -Wl,-Bsymbolic-functions  -L/usr/lib/i386-linux-gnu  `wx-config --libs`  -larchive -lssl -lcrypto


OBJ = net.o disk.o installer.o archive.o settings.o progress.o backup.o md5.o

RELEASE_NAME = gnublin-installer-$(VERSION)
RELEASE_FILES = gnublin-installer settings.xml

gnublin-installer: $(OBJ)
	$(CPP) $(CXXFLAGS) -o gnublin-installer $(OBJ) $(LDFLAGS)


%.o: %.cpp
	$(CPP) $(CXXFLAGS) -c $<

%.o: %.c
	$(CPP) $(CXXFLAGS) -c $<

clean: 
	rm -f *.o
	rm -f gnublin-installer*
astyle:
	astyle --style=java --indent=spaces=2 --indent-classes --indent-switches --indent-labels --indent-col1-comments --pad-oper --unpad-paren --add-brackets --convert-tabs --align-pointer=type *.c *.cpp *.h

release: gnublin-installer
	@strip gnublin-installer
	@tar -czf $(RELEASE_NAME)-bin-$(ARCHITECTURE).tar.gz $(RELEASE_FILES)
	@tar -czf $(RELEASE_NAME)-src.tar.gz *.c *.h *.cpp Makefile gui.fbp settings.xml
	# generate deb

	# copy to deb/usr/share/gnublin-installer
	@mkdir -p deb/usr/share/gnublin-installer
	@mkdir -p deb/usr/bin
	@mkdir -p deb/DEBIAN
	@mkdir -p /tmp/gnublin-installer

	@cp ./gnublin-installer deb/usr/share/gnublin-installer
	@cp ./settings.xml deb/usr/share/gnublin-installer

	# generate starter file
	@echo "#!/bin/bash" > deb/usr/bin/gnublin-installer
	@echo "cd /usr/share/gnublin-installer" >> deb/usr/bin/gnublin-installer
	@echo "./gnublin-installer" >> deb/usr/bin/gnublin-installer

	# determine installed size of package
	@DEBSIZE=`du -c -k -s deb/usr/* | tail -n 1 | gawk '/[0-9]/ { print $1 }'`

	# create control file
	@echo "Package: gnublin-installer" > deb/DEBIAN/control
	@echo "Version: $(VERSION)" >> deb/DEBIAN/control
	@echo "Section: devel" >> deb/DEBIAN/control
	@echo "Priority: optional" >> deb/DEBIAN/control
	@echo "Architecture: $(ARCHITECTURE)" >> deb/DEBIAN/control
	@echo "Essential: no" >> deb/DEBIAN/control
	@echo "Depends: libparted0debian1, libcurl3, libwxgtk2.8-0" >> deb/DEBIAN/control
	@echo "Installed-Size: $(DEBSIZE)" >> deb/DEBIAN/control
	@echo "Maintainer: Manuel Liebert <man.liebert@gmail.com>" >> deb/DEBIAN/control
	@echo "Description: Graphical installer for GNUBLIN embedded Linux board" >> deb/DEBIAN/control

	# copy to an ext2/3/4 drive to set permissions
	@cp -r deb/* /tmp/gnublin-installer

	@chmod +x /tmp/gnublin-installer/usr/bin/gnublin-installer

	# build package
	@dpkg -b /tmp/gnublin-installer $(RELEASE_NAME)-$(ARCHITECTURE).deb

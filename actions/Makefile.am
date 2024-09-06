EXTRA_DIST = build_deb.sh build_rpm.sh

DEBUILD_ARGS =
SAVE_CONTAINER = no

clean-local:
	-rm -rf build/

.PHONY: actions_build_deb
actions_build_deb: IMAGE_NAME = debian
actions_build_deb: IMAGE_VERSION = unstable
actions_build_deb: CONTAINER_NAME = $(PACKAGE_NAME)_$@_$(IMAGE_NAME)-$(IMAGE_VERSION)
actions_build_deb: ../$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz build_deb.sh
	podman pull docker.io/library/$(IMAGE_NAME):$(IMAGE_VERSION)
	podman container exists "$(CONTAINER_NAME)" || podman run -itd --name "$(CONTAINER_NAME)" "$(IMAGE_NAME):$(IMAGE_VERSION)"
	podman exec "$(CONTAINER_NAME)" mkdir -p /tmp/build
	podman cp "../$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz" "$(CONTAINER_NAME)":/tmp/build
	podman cp "build_deb.sh" "$(CONTAINER_NAME)":/tmp/build
	podman exec -w /tmp/build "$(CONTAINER_NAME)" ./build_deb.sh $(IMAGE_NAME) $(IMAGE_VERSION) $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz $(DEBUILD_ARGS)
	mkdir -p "build/$@/$(IMAGE_NAME)-$(IMAGE_VERSION)"
	podman cp "$(CONTAINER_NAME)":/tmp/build/. "build/$@/$(IMAGE_NAME)-$(IMAGE_VERSION)/"
	if [ "$(SAVE_CONTAINER)" = "no" ]; then podman kill "$(CONTAINER_NAME)" && podman rm "$(CONTAINER_NAME)"; fi

.PHONY: actions_build_rpm
actions_build_rpm: IMAGE_NAME = fedora
actions_build_rpm: IMAGE_VERSION = rawhide
actions_build_rpm: CONTAINER_NAME = $(PACKAGE_NAME)_$@_$(IMAGE_NAME)-$(IMAGE_VERSION)
actions_build_rpm: ../$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz build_rpm.sh
	podman pull docker.io/library/$(IMAGE_NAME):$(IMAGE_VERSION)
	podman container exists "$(CONTAINER_NAME)" || podman run -itd --name "$(CONTAINER_NAME)" "$(IMAGE_NAME):$(IMAGE_VERSION)"
	podman exec "$(CONTAINER_NAME)" mkdir -p /root/rpmbuild/SOURCES
	podman exec "$(CONTAINER_NAME)" mkdir -p /root/rpmbuild/SPECS
	podman cp "../$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz" "$(CONTAINER_NAME)":/root/rpmbuild/SOURCES/
	podman cp "../spec/$(PACKAGE_NAME).spec" "$(CONTAINER_NAME)":/root/rpmbuild/SPECS/
	podman cp "build_rpm.sh" "$(CONTAINER_NAME)":/root/
	podman exec "$(CONTAINER_NAME)" /root/build_rpm.sh $(IMAGE_NAME) $(IMAGE_VERSION) $(PACKAGE_NAME).spec
	mkdir -p "build/$@/$(IMAGE_NAME)-$(IMAGE_VERSION)"
	podman cp "$(CONTAINER_NAME)":/root/rpmbuild/. "build/$@/$(IMAGE_NAME)-$(IMAGE_VERSION)/"
	if [ "$(SAVE_CONTAINER)" = "no" ]; then podman kill "$(CONTAINER_NAME)" && podman rm "$(CONTAINER_NAME)"; fi

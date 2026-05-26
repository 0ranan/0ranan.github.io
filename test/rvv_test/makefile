# CC is assigned with = because GNU make predefines CC=cc, so ?= would not apply.
CC       = /opt/riscv64/bin/riscv64-unknown-linux-gnu-gcc
QEMU     ?= qemu-riscv64
SYSROOT  ?= /opt/riscv64/sysroot
CFLAGS   ?= -Wall -Wextra -O2 -march=rv64gcv
QEMU_FLAGS ?= -cpu rv64,v=true

TESTS     := test1 test2 test3 test4 test5
BINS      := $(addprefix build/,$(TESTS))
PASS_MARK := ✅ 测试通过！

.PHONY: all build test clean help $(TESTS)

all: build test

build: $(BINS)

build/test%: test%.c
	@mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

define run_one_test
	@echo "=== Running $(1) ==="
	@out=$$($(QEMU) $(QEMU_FLAGS) -L $(SYSROOT) build/$(1) 2>&1); \
	echo "$$out"; \
	if echo "$$out" | grep -q '$(PASS_MARK)'; then \
		echo "$(1): PASS"; \
	else \
		echo "$(1): FAIL"; \
		exit 1; \
	fi
endef

test: build
	@failed=0; \
	for t in $(TESTS); do \
		echo "=== Running $$t ==="; \
		out=$$($(QEMU) $(QEMU_FLAGS) -L $(SYSROOT) build/$$t 2>&1); \
		echo "$$out"; \
		if echo "$$out" | grep -q '$(PASS_MARK)'; then \
			echo "$$t: PASS"; \
		else \
			echo "$$t: FAIL"; \
			failed=$$((failed + 1)); \
		fi; \
		echo; \
	done; \
	if [ $$failed -gt 0 ]; then \
		echo "$$failed test(s) failed."; \
		exit 1; \
	else \
		echo "All tests passed."; \
	fi

$(TESTS): build
	@$(call run_one_test,$@)

clean:
	rm -rf build

help:
	@echo "RVV test Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all (default)  Build and run all tests"
	@echo "  build          Compile all test binaries into build/"
	@echo "  test           Run all tests under qemu-riscv64"
	@echo "  test1..test5   Build and run a single test"
	@echo "  clean          Remove build/"
	@echo "  help           Show this message"
	@echo ""
	@echo "Variables (override via environment or command line):"
	@echo "  CC=$(CC)"
	@echo "  QEMU=$(QEMU)"
	@echo "  SYSROOT=$(SYSROOT)"
	@echo "  CFLAGS=$(CFLAGS)"
	@echo "  QEMU_FLAGS=$(QEMU_FLAGS)"

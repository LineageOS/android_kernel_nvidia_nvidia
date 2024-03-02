ifeq ($(shell expr $(VERSION) \< 5), 1)
obj-$(CONFIG_SND_SOC) += tegra-alt/
obj-$(CONFIG_SND_SOC) += tegra-alt-sphynx/
else
obj-$(CONFIG_SND_SOC) += tegra-virt-alt/
endif

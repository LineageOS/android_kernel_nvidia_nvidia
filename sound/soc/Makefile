ifeq ($(shell expr $(VERSION) \< 5), 1)
obj-$(CONFIG_SND_SOC) += tegra-alt/
ifdef CONFIG_SND_SOC_TEGRA_ALT_SPHYNX
obj-$(CONFIG_SND_SOC) += tegra-alt-sphynx/
endif
else
obj-$(CONFIG_SND_SOC) += tegra-virt-alt/
endif

#include "OpenXRStereo.h"

VkSurfaceKHR OpenXRStereo::createSurface(std::shared_ptr<VulkanContext> context) {
}

std::array<bool, 3> OpenXRStereo::getMouseButton() {
    return RenderingTarget::getMouseButton();
}

std::vector<std::string> OpenXRStereo::getRequiredInstanceExtensions() {
    return configuration.instanceExtensions;
}

std::vector<std::string> OpenXRStereo::getRequiredDeviceExtensions() {
    return configuration.deviceExtensions;
}

std::pair<uint32_t, uint32_t> OpenXRStereo::getFramebufferSize() const {
    return RenderingTarget::getFramebufferSize();
}

std::array<double, 2> OpenXRStereo::getCursorTranslation() {
    return RenderingTarget::getCursorTranslation();
}

std::array<bool, 7> OpenXRStereo::getKeys() {
    return RenderingTarget::getKeys();
}

void OpenXRStereo::mouseCapture(bool capture) {
    RenderingTarget::mouseCapture(capture);
}

bool OpenXRStereo::tick() {
    return RenderingTarget::tick();
}

void OpenXRStereo::logTranslation(float x, float y) {
    RenderingTarget::logTranslation(x, y);
}

void OpenXRStereo::logMovement(float x, float y) {
    RenderingTarget::logMovement(x, y);
}

std::optional<vk::PhysicalDevice> OpenXRStereo::requirePhysicalDevice(vk::Instance instance) {
    void *pdPtr = configuration.getPhysicalDevice(instance);
    return {static_cast<vk::PhysicalDevice>(static_cast<VkPhysicalDevice>(pdPtr))};
}

void OpenXRStereo::postVulkanInit(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device,
    uint32_t queueFamily, uint32_t queueIndex) {
    configuration.postVulkanInit(instance, physicalDevice, device, queueFamily, queueIndex);
}

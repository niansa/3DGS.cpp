#include <filesystem>
#include <iostream>
#include <libenvpp/env.hpp>

#include "3dgs.h"
#include "args.hxx"
#include "spdlog/spdlog.h"

int main(int argc, char** argv)
{
    spdlog::set_pattern("[%H:%M:%S] [%^%L%$] %v");

    args::ArgumentParser parser("Vulkan Splatting");
    args::HelpFlag helpFlag{parser, "help", "Display this help menu", {'h', "help"}};
    args::Flag validationLayersFlag{
        parser, "validation-layers", "Enable Vulkan validation layers", {"validation"}
    };
    args::Flag verboseFlag{parser, "verbose", "Enable verbose logging", {'v', "verbose"}};
    args::ValueFlag<uint8_t> physicalDeviceIdFlag{
        parser, "physical-device", "Select physical device by index", {'d', "device"}
    };
    args::Flag immediateSwapchainFlag{
        parser, "immediate-swapchain", "Set swapchain mode to immediate (VK_PRESENT_MODE_IMMEDIATE_KHR)",
        {'i', "immediate-swapchain"}
    };
    args::ValueFlag<int> widthFlag{parser, "width", "Set window width", {"width"}};
    args::ValueFlag<int> heightFlag{parser, "height", "Set window height", {"height"}};
    args::Flag noGuiFlag{parser, "no-gui", "Disable GUI", {"no-gui"}};
    args::Positional<std::string> scenePath{parser, "scene", "Path to scene file", "scene.ply"};

    args::ValueFlag<std::string> benchmarkOutputFolderFlag{
        parser, "benchmark-output", "Output folder for benchmark results", {'b', "benchmark-output"}
    };

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Completion& e)
    {
        std::cout << e.what();
        return 0;
    }
    catch (const args::Help&)
    {
        std::cout << parser;
        return 0;
    }
    catch (const args::ParseError& e)
    {
        std::cout << e.what() << std::endl;
        std::cout << parser;
        return 1;
    }

    auto pre = env::prefix("VKGS");
    auto validationLayers = pre.register_variable<bool>("VALIDATION_LAYERS");
    auto physicalDeviceId = pre.register_variable<uint8_t>("PHYSICAL_DEVICE");
    auto immediateSwapchainEnv = pre.register_variable<bool>("IMMEDIATE_SWAPCHAIN");
    auto envVars = pre.parse_and_validate();

    if (args::get(verboseFlag))
    {
        spdlog::set_level(spdlog::level::debug);
    }

    bool immediateSwapchain = envVars.get_or(immediateSwapchainEnv, false);

    VulkanSplatting::RendererConfiguration config{
        envVars.get_or(validationLayers, false),
        envVars.get(physicalDeviceId).has_value()
            ? std::make_optional(envVars.get(physicalDeviceId).value())
            : std::nullopt,
        args::get(scenePath)
    };

    // check that the scene file exists
    if (!std::filesystem::exists(config.scene))
    {
        spdlog::critical("File does not exist: {}", config.scene);
        return 0;
    }

    if (validationLayersFlag)
    {
        config.enableVulkanValidationLayers = args::get(validationLayersFlag);
    }

    if (physicalDeviceIdFlag)
    {
        config.physicalDeviceId = std::make_optional<uint8_t>(args::get(physicalDeviceIdFlag));
    }

    if (immediateSwapchainFlag)
    {
        immediateSwapchain = args::get(immediateSwapchainFlag);
    }

    if (noGuiFlag)
    {
        config.enableGui = false;
    }
    else
    {
        config.enableGui = true;
    }

    if (benchmarkOutputFolderFlag)
    {
        config.benchmarkOutput = std::make_optional(args::get(benchmarkOutputFolderFlag));
    }

    auto width = widthFlag ? args::get(widthFlag) : 1280;
    auto height = heightFlag ? args::get(heightFlag) : 720;

#ifndef DEBUG
    try {
#endif
    auto renderer = VulkanSplatting(std::move(config), VulkanSplatting::createGlfwWindow("Vulkan Splatting", width, height, immediateSwapchain, config.benchmarkOutput.has_value()));
    renderer.start();
#ifndef DEBUG
    } catch (const std::exception& e) {
        spdlog::critical(e.what());
        std::cout << e.what() << std::endl;
        return 0;
    }
#endif
    return 0;
}

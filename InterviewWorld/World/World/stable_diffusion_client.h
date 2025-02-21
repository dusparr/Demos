#ifndef STABLE_DIFFUSION_CLIENT_H
#define STABLE_DIFFUSION_CLIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>

struct StableDiffusionRequest {
    std::string prompt;
    std::string negative_prompt;
    std::vector<std::string> styles;
    int seed = -1;
    int subseed = -1;
    double subseed_strength = 0;
    int seed_resize_from_h = -1;
    int seed_resize_from_w = -1;
    std::string sampler_name = "Euler";
    std::string scheduler = "";
    int batch_size = 1;
    int n_iter = 1;
    int steps = 50;
    double cfg_scale = 7;
    double distilled_cfg_scale = 3.5;
    int width = 512;
    int height = 512;
    bool restore_faces = true;
    bool tiling = true;
    bool do_not_save_samples = false;
    bool do_not_save_grid = false;
    double eta = 0;
    double denoising_strength = 0;
    double s_min_uncond = 0;
    double s_churn = 0;
    double s_tmax = 0;
    double s_tmin = 0;
    double s_noise = 0;
    std::unordered_map<std::string, std::string> override_settings;
    bool override_settings_restore_afterwards = true;
    std::string refiner_checkpoint = "";
    double refiner_switch_at = 0;
    bool disable_extra_networks = false;
    std::unordered_map<std::string, std::string> comments;
    bool enable_hr = false;
    int firstphase_width = 0;
    int firstphase_height = 0;
    double hr_scale = 2;
    std::string hr_upscaler = "";
    int hr_second_pass_steps = 0;
    int hr_resize_x = 0;
    int hr_resize_y = 0;
    std::string hr_checkpoint_name = "";
    std::vector<std::string> hr_additional_modules;
    std::string hr_sampler_name = "";
    std::string hr_scheduler = "";
    std::string hr_prompt = "";
    std::string hr_negative_prompt = "";
    double hr_cfg = 1;
    double hr_distilled_cfg = 3.5;
    std::string force_task_id = "";
    std::string sampler_index = "Euler";
    std::string script_name = "";
    std::vector<std::string> script_args;
    bool send_images = true;
    bool save_images = false;
    std::unordered_map<std::string, std::string> alwayson_scripts;
    std::string infotext = "";

    //  Convert struct to JSON
    friend void to_json(nlohmann::json& j, const StableDiffusionRequest& request) {
        j = {
            {"prompt", request.prompt},
            {"negative_prompt", request.negative_prompt},
            {"styles", request.styles},
            {"seed", request.seed},
            {"subseed", request.subseed},
            {"subseed_strength", request.subseed_strength},
            {"seed_resize_from_h", request.seed_resize_from_h},
            {"seed_resize_from_w", request.seed_resize_from_w},
            {"sampler_name", request.sampler_name},
            {"scheduler", request.scheduler},
            {"batch_size", request.batch_size},
            {"n_iter", request.n_iter},
            {"steps", request.steps},
            {"cfg_scale", request.cfg_scale},
            {"distilled_cfg_scale", request.distilled_cfg_scale},
            {"width", request.width},
            {"height", request.height},
            {"restore_faces", request.restore_faces},
            {"tiling", request.tiling},
            {"do_not_save_samples", request.do_not_save_samples},
            {"do_not_save_grid", request.do_not_save_grid},
            {"eta", request.eta},
            {"denoising_strength", request.denoising_strength},
            {"s_min_uncond", request.s_min_uncond},
            {"s_churn", request.s_churn},
            {"s_tmax", request.s_tmax},
            {"s_tmin", request.s_tmin},
            {"s_noise", request.s_noise},
            {"override_settings", request.override_settings},
            {"override_settings_restore_afterwards", request.override_settings_restore_afterwards},
            {"refiner_checkpoint", request.refiner_checkpoint},
            {"refiner_switch_at", request.refiner_switch_at},
            {"disable_extra_networks", request.disable_extra_networks},
            {"comments", request.comments},
            {"enable_hr", request.enable_hr},
            {"firstphase_width", request.firstphase_width},
            {"firstphase_height", request.firstphase_height},
            {"hr_scale", request.hr_scale},
            {"hr_upscaler", request.hr_upscaler},
            {"hr_second_pass_steps", request.hr_second_pass_steps},
            {"hr_resize_x", request.hr_resize_x},
            {"hr_resize_y", request.hr_resize_y},
            {"hr_checkpoint_name", request.hr_checkpoint_name},
            {"hr_additional_modules", request.hr_additional_modules},
            {"hr_sampler_name", request.hr_sampler_name},
            {"hr_scheduler", request.hr_scheduler},
            {"hr_prompt", request.hr_prompt},
            {"hr_negative_prompt", request.hr_negative_prompt},
            {"hr_cfg", request.hr_cfg},
            {"hr_distilled_cfg", request.hr_distilled_cfg},
            {"force_task_id", request.force_task_id},
            {"sampler_index", request.sampler_index},
            {"script_name", request.script_name},
            {"script_args", request.script_args},
            {"send_images", request.send_images},
            {"save_images", request.save_images},
            {"alwayson_scripts", request.alwayson_scripts},
            {"infotext", request.infotext}
        };
    }

    //  Deserialize JSON to struct
    friend void from_json(const nlohmann::json& j, StableDiffusionRequest& request) {
        request.prompt = j.value("prompt", request.prompt);
        request.negative_prompt = j.value("negative_prompt", request.negative_prompt);
        request.styles = j.value("styles", request.styles);
        request.seed = j.value("seed", request.seed);
        request.subseed = j.value("subseed", request.subseed);
        request.subseed_strength = j.value("subseed_strength", request.subseed_strength);
        request.seed_resize_from_h = j.value("seed_resize_from_h", request.seed_resize_from_h);
        request.seed_resize_from_w = j.value("seed_resize_from_w", request.seed_resize_from_w);
        request.sampler_name = j.value("sampler_name", request.sampler_name);
        request.scheduler = j.value("scheduler", request.scheduler);
        request.batch_size = j.value("batch_size", request.batch_size);
        request.n_iter = j.value("n_iter", request.n_iter);
        request.steps = j.value("steps", request.steps);
        request.cfg_scale = j.value("cfg_scale", request.cfg_scale);
        request.distilled_cfg_scale = j.value("distilled_cfg_scale", request.distilled_cfg_scale);
        request.width = j.value("width", request.width);
        request.height = j.value("height", request.height);
        request.restore_faces = j.value("restore_faces", request.restore_faces);
        request.tiling = j.value("tiling", request.tiling);
        request.do_not_save_samples = j.value("do_not_save_samples", request.do_not_save_samples);
        request.do_not_save_grid = j.value("do_not_save_grid", request.do_not_save_grid);
        request.eta = j.value("eta", request.eta);
        request.denoising_strength = j.value("denoising_strength", request.denoising_strength);
        request.s_min_uncond = j.value("s_min_uncond", request.s_min_uncond);
        request.s_churn = j.value("s_churn", request.s_churn);
        request.s_tmax = j.value("s_tmax", request.s_tmax);
        request.s_tmin = j.value("s_tmin", request.s_tmin);
        request.s_noise = j.value("s_noise", request.s_noise);

        // Handle dictionaries/maps properly
        if (j.contains("override_settings")) {
            request.override_settings = j.at("override_settings").get<std::unordered_map<std::string, std::string>>();
        }
        request.override_settings_restore_afterwards = j.value("override_settings_restore_afterwards", request.override_settings_restore_afterwards);

        request.refiner_checkpoint = j.value("refiner_checkpoint", request.refiner_checkpoint);
        request.refiner_switch_at = j.value("refiner_switch_at", request.refiner_switch_at);
        request.disable_extra_networks = j.value("disable_extra_networks", request.disable_extra_networks);
        //request.firstpass_image = j.value("firstpass_image", request.firstpass_image);

        // Handle comments map properly
        if (j.contains("comments")) {
            request.comments = j.at("comments").get<std::unordered_map<std::string, std::string>>();
        }

        request.enable_hr = j.value("enable_hr", request.enable_hr);
        request.firstphase_width = j.value("firstphase_width", request.firstphase_width);
        request.firstphase_height = j.value("firstphase_height", request.firstphase_height);
        request.hr_scale = j.value("hr_scale", request.hr_scale);
        request.hr_upscaler = j.value("hr_upscaler", request.hr_upscaler);
        request.hr_second_pass_steps = j.value("hr_second_pass_steps", request.hr_second_pass_steps);
        request.hr_resize_x = j.value("hr_resize_x", request.hr_resize_x);
        request.hr_resize_y = j.value("hr_resize_y", request.hr_resize_y);
        request.hr_checkpoint_name = j.value("hr_checkpoint_name", request.hr_checkpoint_name);

        // Handle lists properly
        if (j.contains("hr_additional_modules")) {
            request.hr_additional_modules = j.at("hr_additional_modules").get<std::vector<std::string>>();
        }
        request.hr_sampler_name = j.value("hr_sampler_name", request.hr_sampler_name);
        request.hr_scheduler = j.value("hr_scheduler", request.hr_scheduler);
        request.hr_prompt = j.value("hr_prompt", request.hr_prompt);
        request.hr_negative_prompt = j.value("hr_negative_prompt", request.hr_negative_prompt);
        request.hr_cfg = j.value("hr_cfg", request.hr_cfg);
        request.hr_distilled_cfg = j.value("hr_distilled_cfg", request.hr_distilled_cfg);
        request.force_task_id = j.value("force_task_id", request.force_task_id);
        request.sampler_index = j.value("sampler_index", request.sampler_index);
        request.script_name = j.value("script_name", request.script_name);

        // Handle script args list properly
        if (j.contains("script_args")) {
            request.script_args = j.at("script_args").get<std::vector<std::string>>();
        }

        request.send_images = j.value("send_images", request.send_images);
        request.save_images = j.value("save_images", request.save_images);

        // Handle alwayson_scripts properly
        if (j.contains("alwayson_scripts")) {
            request.alwayson_scripts = j.at("alwayson_scripts").get<std::unordered_map<std::string, std::string>>();
        }

        request.infotext = j.value("infotext", request.infotext);
    }


    //  Load struct from JSON file
    static StableDiffusionRequest from_json_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON file: " + filename);
        }

        nlohmann::json jsonData;
        file >> jsonData;
        file.close();

        StableDiffusionRequest request;  // Keeps defaults
        from_json(jsonData, request);   // Only updates existing fields
        return request;
    }
};

class StableDiffusionClient {
private:
    std::string baseURL;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    std::vector<unsigned char> DecodeBase64(const std::string& base64Data);

public:
    StableDiffusionClient(const std::string& apiURL);

    //  Generate an image from a text prompt
    std::string GenerateImage(const StableDiffusionRequest& request);

    //  Save Base64 image data to a file
    bool SaveBase64Image(const std::string& base64Data, const std::string& filePath);
};

#endif // STABLE_DIFFUSION_CLIENT_H

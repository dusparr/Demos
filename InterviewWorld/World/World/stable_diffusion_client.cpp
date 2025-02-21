#include "stable_diffusion_client.h"
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include "stb_image_write.h"  //  Include stb_image_write for PNG saving


using json = nlohmann::json;
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
//  cURL Write Callback
size_t StableDiffusionClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

//  Constructor
StableDiffusionClient::StableDiffusionClient(const std::string& apiURL) : baseURL(apiURL) {}

//  Send API Request to Stable Diffusion
std::string StableDiffusionClient::GenerateImage(const StableDiffusionRequest& request) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL.\n";
        return "";
    }

    //  Convert `StableDiffusionRequest` to JSON properly
    nlohmann::json jsonPayload;
    to_json(jsonPayload, request);
    std::string jsonString = jsonPayload.dump();

    std::string responseBuffer;
    std::string api = baseURL;
    curl_easy_setopt(curl, CURLOPT_URL, api.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cout << "CURL error: " << curl_easy_strerror(res) << "\n";
        return "";
    }
    std::string base64Image;
    try {
        nlohmann::json jsonResponse = nlohmann::json::parse(responseBuffer);
        if (jsonResponse.contains("images") && jsonResponse["images"].is_array() && !jsonResponse["images"].empty()) {
            base64Image = jsonResponse["images"][0].get<std::string>(); 
        }
        else {
            std::cerr << "Error: No image found in response.\n";
            return "";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "JSON Parse Error: " << e.what() << "\n";
        return "";
    }


    return base64Image;
}


std::vector<unsigned char> StableDiffusionClient::DecodeBase64(const std::string& base64Data) {
    int in_len = base64Data.size();
    int i = 0, j = 0, in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<unsigned char> decodedData;

    while (in_len-- && (base64Data[in_] != '=') && (isalnum(base64Data[in_]) || base64Data[in_] == '+' || base64Data[in_] == '/')) {
        char_array_4[i++] = base64Data[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++)
                decodedData.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; j < i - 1; j++)
            decodedData.push_back(char_array_3[j]);
    }

    return decodedData;
}

//  Save decoded Base64 image data to PNG
bool StableDiffusionClient::SaveBase64Image(const std::string& base64Data, const std::string& filePath) {
    // Decode Base64
    std::vector<unsigned char> imageData = DecodeBase64(base64Data);
    if (imageData.empty()) {
        std::cerr << "Failed to decode Base64 image.\n";
        return false;
    }

    // Save to file using stb_image_write
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filePath << "\n";
        return false;
    }

    file.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());
    file.close();

    std::cout << "Image saved to " << filePath << "\n";
    return true;
}

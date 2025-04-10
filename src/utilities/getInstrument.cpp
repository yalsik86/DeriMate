#include "getInstrument.hpp"

size_t WriteCallback6(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

std::string getInstrument(std::string& currency) {
    CURL* curl;
    CURLcode res;
    std::string response_buffer;
    std::vector<std::string> instruments;

    spdlog::info("Fetching instruments for currency: {}", currency);

    std::vector<std::string> kinds = {"spot", "future", "option"};
    std::cout << "Select an Instrument Type (0 to abort):\n";
    for (size_t i = 0; i < kinds.size(); i++) {
        std::cout << i + 1 << ". " << kinds[i] << "\n";
    }

    int kind_choice;
    std::cout << "Enter choice: ";
    std::cin >> kind_choice;

    if (kind_choice == 0) {
        spdlog::info("User aborted instrument type selection.");
        return "";
    } else if (kind_choice < 1 || kind_choice > kinds.size()) {
        spdlog::error("Invalid instrument type selection.");
        return "";
    }

    std::string selected_kind = kinds[kind_choice - 1];
    std::string url = "https://test.deribit.com/api/v2/public/get_instruments?currency=" + currency + "&kind=" + selected_kind;

    curl = curl_easy_init();
    if (!curl) {
        spdlog::error("Failed to initialize CURL.");
        return "";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback6);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        spdlog::error("Error fetching instruments: {}", curl_easy_strerror(res));
        return "";
    }

    try {
        boost::json::value response_json = boost::json::parse(response_buffer);
        auto result = response_json.at("result").as_array();

        if (result.empty()) {
            spdlog::warn("No instruments found for {} in {} market.", currency, selected_kind);
            return "";
        }

        spdlog::info("Fetched {} instruments.", result.size());

        std::cout << "Select an Instrument (0 to abort):\n";
        int index = 1;
        for (auto& item : result) {
            std::string instrument_name = item.at("instrument_name").as_string().c_str();
            instruments.push_back(instrument_name);
            std::cout << index << ". " << instrument_name << "\n";
            index++;
        }

        int choice;
        std::cout << "Enter choice: ";
        std::cin >> choice;

        if (choice == 0) {
            spdlog::info("User aborted instrument selection.");
            return "";
        } else if (choice > 0 && choice <= instruments.size()) {
            return instruments[choice - 1];
        } else {
            spdlog::error("Invalid instrument selection.");
            return "";
        }
    } catch (const std::exception& e) {
        spdlog::error("JSON Parsing Error: {}", e.what());
        return "";
    }
}

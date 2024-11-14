//
// Created by yawaflua on 01/11/2024.
//
#include <string>
#include <nlohmann/json.hpp>

namespace tzeva_adom {
    class AlertMessage {
    public:
        std::string heb;
        std::string eng;
        std::string rus;
        std::string arb;
        int catId;
        int matrixCatId;
        std::string hebTitle;
        std::string engTitle;
        std::string rusTitle;
        std::string arbTitle;

        // AlertMessage(
        //     const std::string& heb, const std::string& eng, const std::string& rus, const std::string& arb,
        //     int catId, int matrixCatId,
        //     const std::string& hebTitle, const std::string& engTitle,
        //     const std::string& rusTitle, const std::string& arbTitle
        // )
        //     : heb(heb), eng(eng), rus(rus), arb(arb),
        //       catId(catId), matrixCatId(matrixCatId),
        //       hebTitle(hebTitle), engTitle(engTitle),
        //       rusTitle(rusTitle), arbTitle(arbTitle) {}

        AlertMessage() = default;
        virtual ~AlertMessage() = default;



        // JSON сериализация
        friend void to_json(nlohmann::json& j, const AlertMessage& message) {
            j = nlohmann::json{
                    {"heb", message.heb},
                    {"eng", message.eng},
                    {"rus", message.rus},
                    {"arb", message.arb},
                    {"catId", message.catId},
                    {"matrixCatId", message.matrixCatId},
                    {"hebTitle", message.hebTitle},
                    {"engTitle", message.engTitle},
                    {"rusTitle", message.rusTitle},
                    {"arbTitle", message.arbTitle}
            };
        }

        // JSON десериализация
        friend void from_json(const nlohmann::json& j, AlertMessage& message) {
            j.at("heb").get_to(message.heb);
            j.at("eng").get_to(message.eng);
            j.at("rus").get_to(message.rus);
            j.at("arb").get_to(message.arb);
            j.at("catId").get_to(message.catId);
            j.at("matrixCatId").get_to(message.matrixCatId);
            j.at("hebTitle").get_to(message.hebTitle);
            j.at("engTitle").get_to(message.engTitle);
            j.at("rusTitle").get_to(message.rusTitle);
            j.at("arbTitle").get_to(message.arbTitle);
        }

        using AlertingMessages = std::vector<AlertMessage>;
    };
}
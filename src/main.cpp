#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>

using namespace geode::prelude;

struct BuildObject {
    int id;
    float x;
    float y;
    float scale;
    float rot;
};

class $modify(MyAutobuilder, LevelEditorLayer) {
public:
    void placeObject(const BuildObject& objData) {
        auto obj = GameObject::createWithKey(objData.id);
        if (!obj) return;
        
        obj->setPosition({objData.x, objData.y});
        obj->setScale(objData.scale);
        obj->setRotation(objData.rot);
        
        this->addObject(obj);
    }

    void loadTemplate() {
        // Obtiene la ruta de configuracion
        std::filesystem::path configDir = Mod::get()->getConfigDir();
        
        // Si la carpeta no existe, la crea
        if (!std::filesystem::exists(configDir)) {
            std::filesystem::create_directories(configDir);
        }

        std::filesystem::path filePath = configDir / "template.txt";

        // Si el archivo no existe, lo crea con un ejemplo
        if (!std::filesystem::exists(filePath)) {
            std::ofstream outfile(filePath);
            outfile << "# Formato: ID, X, Y, Scale(opcional), Rotation(opcional)\n";
            outfile << "1, 150.0, 150.0, 1.0, 0.0\n";
            outfile << "1, 180.0, 150.0, 1.0, 0.0\n";
            outfile << "8, 210.0, 150.0, 1.0, 0.0\n";
            outfile.close();
            
            FLAlertLayer::create("Autobuild", "Carpeta creada. Archivo template.txt generado. Buscalo en la carpeta de configuracion del mod.", "OK")->show();
            return;
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            FLAlertLayer::create("Error", "No se pudo abrir template.txt", "OK")->show();
            return;
        }

        std::string line;
        int count = 0;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> seglist;

            while(std::getline(ss, segment, ',')) {
                seglist.push_back(segment);
            }

            if (seglist.size() >= 3) {
                BuildObject obj;
                obj.id = std::stoi(seglist[0]);
                obj.x = std::stof(seglist[1]);
                obj.y = std::stof(seglist[2]);
                obj.scale = seglist.size() > 3 ? std::stof(seglist[3]) : 1.0f;
                obj.rot = seglist.size() > 4 ? std::stof(seglist[4]) : 0.0f;
                
                placeObject(obj);
                count++;
            }
        }
        file.close();

        std::string msg = "Se inyectaron " + std::to_string(count) + " objetos.";
        FLAlertLayer::create("Autobuild", msg.c_str(), "OK")->show();
    }

    void onKeyDown(cocos2d::enumKeyCodes key, double p1) {
        LevelEditorLayer::onKeyDown(key, p1);
        
        // Tecla B para activar el Autobuild
        if (key == cocos2d::KEY_B) { 
            loadTemplate();
        }
    }
};

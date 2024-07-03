#ifndef SLSCRIPT_H
#define SLSCRIPT_H

#include "SLScriptInterface.h"

#include "banjo/emit/binary_module.hpp"
#include "banjo/target/target_description.hpp"
#include "banjo/target/target.hpp"

#include <cstdint>
#include <string>

//-----------------------------------------------------------------------------
class SLScript
{
public:
    SLScript(std::string sourceDir);
    ~SLScript();

    void load();
    void unload();
    void reload();

    void onUpdate(SLScene* scene, SLSceneID sceneID);
    bool onKeyPress(int key);
    bool onKeyRelease(int key);

private:
    typedef void (*OnInit)(SLScriptContext* context);
    typedef void (*OnUpdate)(SLScriptContext* context, float deltaTime);
    typedef bool (*OnKeyPress)(SLScriptContext* context, int key);
    typedef bool (*OnKeyRelease)(SLScriptContext* context, int key);

    char* allocSection(const banjo::WriteBuffer& buffer);
    void  writeSection(char* section, const banjo::WriteBuffer& buffer, unsigned long protect);
    void  freeSection(char* section, const banjo::WriteBuffer& buffer);
    char* findFunction(const std::string& name);

    banjo::target::TargetDescription _targetDescr;
    banjo::target::Target*           _target;

    banjo::BinModule _binMod;
    char*            _textBase;
    std::uint32_t    _textSize;
    char*            _dataBase;
    std::uint32_t    _dataSize;

    SLScriptContext _context;
    OnInit          _onInit;
    OnUpdate        _onUpdate;
    OnKeyPress      _onKeyPress;
    OnKeyRelease    _onKeyRelease;
};
//-----------------------------------------------------------------------------

#endif
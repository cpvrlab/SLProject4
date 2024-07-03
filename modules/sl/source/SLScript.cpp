#include "SLScript.h"

#include "SLScriptInterface.h"

#include "banjo/codegen/machine_pass_runner.hpp"
#include "banjo/config/config.hpp"
#include "banjo/emit/binary_module.hpp"
#include "banjo/emit/nasm_emitter.hpp"
#include "banjo/ir_builder/root_ir_builder.hpp"
#include "banjo/reports/report_manager.hpp"
#include "banjo/reports/report_printer.hpp"
#include "banjo/sema/semantic_analyzer.hpp"
#include "banjo/source/file_module_loader.hpp"
#include "banjo/source/module_manager.hpp"
#include "banjo/symbol/data_type_manager.hpp"
#include "banjo/target/target.hpp"
#include "banjo/target/target_description.hpp"
#include "banjo/codegen/ir_lowerer.hpp"
#include "banjo/target/x86_64/x86_64_encoder.hpp"

#include "SLScene.h"

#include <cstddef>
#include <cstring>
#include <cmath>

#include <windows.h>

//-----------------------------------------------------------------------------
extern "C" void ___banjo_print_string();
extern "C" void ___banjo_print_address();
extern "C" void ___banjo_f64_to_string();
extern "C" void ___acquire_stack_trace();
//-----------------------------------------------------------------------------
#define PATCH_FUNCTION(name) {#name, reinterpret_cast<char*>(name)}
//-----------------------------------------------------------------------------
std::unordered_map<std::string, char*> patchFunctions = {
  PATCH_FUNCTION(memcpy),

  PATCH_FUNCTION(___banjo_print_string),
  PATCH_FUNCTION(___banjo_print_address),
  PATCH_FUNCTION(___banjo_f64_to_string),
  PATCH_FUNCTION(___acquire_stack_trace),

  PATCH_FUNCTION(GetLastError),
  PATCH_FUNCTION(FormatMessageA),
  PATCH_FUNCTION(LocalFree),
  PATCH_FUNCTION(__acrt_iob_func),
  PATCH_FUNCTION(CreateMutexA),
  PATCH_FUNCTION(WaitForSingleObject),
  PATCH_FUNCTION(ReleaseMutex),
  PATCH_FUNCTION(CloseHandle),
  PATCH_FUNCTION(WaitForSingleObject),
  PATCH_FUNCTION(WriteFile),
  PATCH_FUNCTION(CreateThread),
  PATCH_FUNCTION(WaitForSingleObject),
  PATCH_FUNCTION(Sleep),
  PATCH_FUNCTION(GetNativeSystemInfo),
  PATCH_FUNCTION(GetSystemTimeAsFileTime),
  PATCH_FUNCTION(QueryPerformanceCounter),
  PATCH_FUNCTION(QueryPerformanceFrequency),
  PATCH_FUNCTION(fopen),
  PATCH_FUNCTION(fseek),
  PATCH_FUNCTION(ftell),
  PATCH_FUNCTION(fseek),
  PATCH_FUNCTION(fread),
  PATCH_FUNCTION(fseek),
  PATCH_FUNCTION(ftell),
  PATCH_FUNCTION(fseek),
  PATCH_FUNCTION(fread),
  PATCH_FUNCTION(fclose),
  PATCH_FUNCTION(fread),
  PATCH_FUNCTION(ferror),
  PATCH_FUNCTION(fgetc),
  PATCH_FUNCTION(fputc),
  PATCH_FUNCTION(fwrite),
  PATCH_FUNCTION(strlen),
  PATCH_FUNCTION(fwrite),
  PATCH_FUNCTION(fflush),
  PATCH_FUNCTION(atof),
  PATCH_FUNCTION(sqrtf),
  PATCH_FUNCTION(sinf),
  PATCH_FUNCTION(cosf),
  PATCH_FUNCTION(tanf),
  PATCH_FUNCTION(malloc),
  PATCH_FUNCTION(realloc),
  PATCH_FUNCTION(free),
  PATCH_FUNCTION(memset),
  PATCH_FUNCTION(printf),
  PATCH_FUNCTION(strcmp),
  PATCH_FUNCTION(strlen),
  PATCH_FUNCTION(strlen),
  PATCH_FUNCTION(strtol),
  PATCH_FUNCTION(strtof),
  PATCH_FUNCTION(strcmp),
  PATCH_FUNCTION(strcmp),
  PATCH_FUNCTION(exit),
  PATCH_FUNCTION(_errno),
  PATCH_FUNCTION(strerror),

  PATCH_FUNCTION(slScriptContextUserData),
  PATCH_FUNCTION(slScriptContextSetUserData),
  PATCH_FUNCTION(slScriptContextScene),
  PATCH_FUNCTION(slScriptContextSceneID),
  PATCH_FUNCTION(slScriptSceneName),
  PATCH_FUNCTION(slScriptSceneFindNodeByName),
  PATCH_FUNCTION(slScriptNodePosition),
  PATCH_FUNCTION(slScriptNodeSetPosition),
  PATCH_FUNCTION(slScriptNodeLookAt),
};
//-----------------------------------------------------------------------------
SLScript::SLScript(std::string sourceDir)
{
    banjo::lang::Config& config = banjo::lang::Config::instance();
    config.paths.push_back(sourceDir);

    _targetDescr = {banjo::target::Architecture::X86_64,
                    banjo::target::OperatingSystem::WINDOWS,
                    banjo::target::Environment::MSVC};

    _target = banjo::target::Target::create(_targetDescr,
                                            banjo::target::CodeModel::LARGE);
}
//-----------------------------------------------------------------------------
SLScript::~SLScript()
{
    delete _target;
}
//-----------------------------------------------------------------------------
void SLScript::load()
{
    ////////////////////
    // Compile script //
    ////////////////////

    // Load and parse modules into ASTs.
    banjo::lang::FileModuleLoader loader;
    banjo::lang::ReportManager    reportManager;
    banjo::lang::ModuleManager    moduleManager(loader, reportManager);
    moduleManager.add_search_path(std::filesystem::path(SL_PROJECT_ROOT) / "modules" / "sl" / "externals" / "banjo-lang" / "lib" / "stdlib");
    moduleManager.add_config_search_paths(banjo::lang::Config::instance());
    moduleManager.load_all();

    // Run semantic analysis over the source code.
    banjo::lang::DataTypeManager  type_manager;
    banjo::lang::SemanticAnalyzer analyzer(moduleManager, type_manager, _target);
    banjo::lang::SemanticAnalysis analysis = analyzer.analyze();
    reportManager.merge_result(analysis.reports, analysis.is_valid);

    // Make sure the script is valid and return otherwise.
    if (!reportManager.is_valid())
    {
        banjo::lang::ReportPrinter reportPrinter(moduleManager);
        reportPrinter.enable_colors();
        reportPrinter.print_reports(reportManager.get_reports());
        return;
    }

    // Build the SSA IR from the AST.
    banjo::ir_builder::RootIRBuilder irBuilder(moduleManager.get_module_list(), _target);
    banjo::ir::Module                irMod = irBuilder.build();

    // Lower the SSA IR to machine IR.
    banjo::codegen::IRLowerer* irLowerer  = _target->create_ir_lowerer();
    banjo::mcode::Module       machineMod = irLowerer->lower_module(irMod);
    delete irLowerer;

    // Run machine-specific passes like register allocation or prolog/epilog insertion.
    banjo::codegen::MachinePassRunner(_target).create_and_run(machineMod);

    // Emit the generated assembly code for debugging.
    // std::ofstream asmFile("script.asm");
    // banjo::codegen::NASMEmitter(machineMod, asmFile, _targetDescr).generate();
    // asmFile.close();

    // Encode the machine IR as x86-64 machine code ( = assembling).
    _binMod = banjo::target::X8664Encoder().encode(machineMod);

    /////////////////////////////////////////////
    // Create text and data sections in memory //
    /////////////////////////////////////////////

    _textSize = _binMod.text.get_size();
    _textBase = allocSection(_binMod.text);

    _dataSize = _binMod.data.get_size();
    _dataBase = allocSection(_binMod.data);

    /////////////////////////////////////
    // Patch relocations in the binary //
    /////////////////////////////////////

    for (banjo::BinSymbolUse& use : _binMod.symbol_uses)
    {
        banjo::BinSymbolDef& def = _binMod.symbol_defs[use.symbol_index];

        if (use.section == banjo::BinSectionKind::TEXT)
        {
            _binMod.text.seek(use.address);

            if (def.kind == banjo::BinSymbolKind::TEXT_FUNC || def.kind == banjo::BinSymbolKind::TEXT_LABEL)
            {
                std::uint64_t address = reinterpret_cast<std::uint64_t>(_textBase) + def.offset + use.addend;
                _binMod.text.write_u64(address);
            }
            else if (def.kind == banjo::BinSymbolKind::DATA_LABEL)
            {
                std::uint64_t address = reinterpret_cast<std::uint64_t>(_dataBase) + def.offset + use.addend;
                _binMod.text.write_u64(address);
            }
            else if (def.kind == banjo::BinSymbolKind::UNKNOWN)
            {
                auto iter = patchFunctions.find(def.name);
                if (iter != patchFunctions.end())
                {
                    std::uint64_t funcAddress = reinterpret_cast<std::uint64_t>(iter->second);
                    _binMod.text.write_u64(funcAddress + use.addend);
                }
                else
                {
                    // std::cout << def.name << std::endl;
                    _binMod.text.write_u64(0);
                }
            }
        }
    }

    /////////////////////////////////////////////
    // Write binary data to sections in memory //
    /////////////////////////////////////////////

    writeSection(_textBase, _binMod.text, PAGE_EXECUTE);
    writeSection(_dataBase, _binMod.data, PAGE_READWRITE);

    ///////////////////////////////////////
    // Find pointers to script functions //
    ///////////////////////////////////////

    _onInit       = reinterpret_cast<OnInit>(findFunction("on_init"));
    _onUpdate     = reinterpret_cast<OnUpdate>(findFunction("on_update"));
    _onKeyPress   = reinterpret_cast<OnKeyPress>(findFunction("on_key_press"));
    _onKeyRelease = reinterpret_cast<OnKeyPress>(findFunction("on_key_release"));

    //////////////////
    // Call on_init //
    //////////////////

    if (_onInit)
        _onInit(&_context);
}
//-----------------------------------------------------------------------------
void SLScript::unload()
{
    freeSection(_textBase, _binMod.text);
    freeSection(_dataBase, _binMod.data);
}
//-----------------------------------------------------------------------------
void SLScript::reload()
{
    unload();
    load();
}
//-----------------------------------------------------------------------------
void SLScript::onUpdate(SLScene* scene, SLSceneID sceneID)
{
    _context.scene   = scene;
    _context.sceneID = sceneID;

    if (_onUpdate)
        _onUpdate(&_context, scene->elapsedTimeSec());
}
//-----------------------------------------------------------------------------
bool SLScript::onKeyPress(int key)
{
    if (_onKeyPress)
        return _onKeyPress(&_context, key);
    else
        return false;
}
//-----------------------------------------------------------------------------
bool SLScript::onKeyRelease(int key)
{
    if (_onKeyRelease)
        return _onKeyRelease(&_context, key);
    else
        return false;
}
//-----------------------------------------------------------------------------
char* SLScript::allocSection(const banjo::WriteBuffer& buffer)
{
    void* section = VirtualAlloc(NULL, buffer.get_size(), MEM_COMMIT, PAGE_READWRITE);
    return reinterpret_cast<char*>(section);
}
//-----------------------------------------------------------------------------
void SLScript::writeSection(char* section, const banjo::WriteBuffer& buffer, unsigned long protect)
{
    std::memcpy(section, buffer.get_data().data(), buffer.get_size());

    DWORD oldProtect;
    VirtualProtect(section, buffer.get_size(), protect, &oldProtect);
}
//-----------------------------------------------------------------------------
void SLScript::freeSection(char* section, const banjo::WriteBuffer& buffer)
{
    VirtualFree(section, buffer.get_size(), MEM_RELEASE);
}
//-----------------------------------------------------------------------------
char* SLScript::findFunction(const std::string& name)
{
    for (const banjo::BinSymbolDef& def : _binMod.symbol_defs)
        if (def.name == name)
            return _textBase + def.offset;

    return nullptr;
}
//-----------------------------------------------------------------------------
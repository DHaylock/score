#pragma once
#include <iscore/application/ApplicationContext.hpp>
#include <QtGlobal>
#include <chrono>

namespace iscore
{
    /**
     * @brief The Command class
     *
     * The base of the command system in i-score
     * It is timestamped, because we can then compare between clients.
     *
     * Maybe the iscore_plugin_network should replace the Command by a TimestampedCommand instead ?
     * What if other plug-ins also want to add functionality ?
     *
     * Note: for mergeWith put two timestamps, one for the initial command (5 sec) and one for each
     * new command merged.
     */
    class ISCORE_LIB_BASE_EXPORT Command
    {
        public:
            Command();
            virtual ~Command();

            virtual void undo() const = 0;
            virtual void redo() const = 0;

            virtual const CommandFactoryKey& key() const = 0;

        protected:
            quint32 timestamp() const;
            void setTimestamp(quint32 stmp);

            const iscore::ApplicationContext& context;

        private:
            //TODO check if this is UTC
            std::chrono::milliseconds m_timestamp
            {  std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch()) };
    };
}

/**
 * This macro is used to specify the common metadata of commands :
 *  - factory name (e.g. "ScenarioApplicationPlugin")
 *  - command name
 *  - command description
 */
#define ISCORE_COMMAND_DECL(parentNameFun, name, desc) \
    public: \
        name() = default; \
        virtual const CommandParentFactoryKey& parentKey() const override { return parentNameFun; } \
        virtual const CommandFactoryKey& key() const override { return static_key(); } \
        virtual QString description() const override { return QObject::tr(desc); }  \
    static const CommandFactoryKey& static_key() \
    { \
        static const CommandFactoryKey var{#name}; \
        return var; \
    } \
    private:

// A helper to allow cmake to parse commands.
#define ISCORE_COMMAND_DECL_T(name)

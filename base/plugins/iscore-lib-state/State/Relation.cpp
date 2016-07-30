#include <State/ValueConversion.hpp>
#include <QMap>

#include "Relation.hpp"

namespace State {
struct Address;
struct Value;

QString toString(const Pulse& pulse)
{
    return pulse.address.toString() + " impulse";
}

}  // namespace iscore

QString State::toString(const State::RelationMember& m)
{
    struct {
        public:
            using return_type = QString;
            return_type operator()(const State::Address& addr) const {
                return addr.toString();
            }

            return_type operator()(const State::Value& val) const {
                return State::convert::toPrettyString(val);
            }

            return_type operator()(const State::AddressAccessor& acc) const {
                auto addr = acc.address.toString();
                for(auto val : acc.accessors)
                {
                    addr += QString("[%1]").arg(val);
                }
                return addr;
            }
    } visitor{};

    return eggs::variants::apply(visitor, m);
}

QString State::toString(const Relation& rel)
{
    using namespace eggs::variants;

    return QString("%1 %2 %3")
            .arg(toString(rel.lhs))
            .arg(opToString()[rel.op])
            .arg(toString(rel.rhs));
}

const QMap<State::Relation::Comparator, QString> State::opToString()
{
    return {
        {State::Relation::Comparator::LowerEqual,    "<="},
        {State::Relation::Comparator::GreaterEqual,  ">="},
        {State::Relation::Comparator::Lower,         "<"},
        {State::Relation::Comparator::Greater,       ">"},
        {State::Relation::Comparator::Different,     "!="},
        {State::Relation::Comparator::Equal,         "=="},
        {State::Relation::Comparator::None,          ""}
    };
}

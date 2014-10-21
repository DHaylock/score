#include <core/model/Model.hpp>
#include <interface/panels/Panel.hpp>

using namespace iscore;

void Model::addPanel(PanelModel* m)
{
	m->setParent(this);
	m_panelsModels.insert(m);
}

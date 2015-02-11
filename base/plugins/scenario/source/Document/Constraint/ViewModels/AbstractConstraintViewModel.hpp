#pragma once
#include <core/tools/IdentifiedObject.hpp>

class ConstraintModel;
class BoxModel;
class AbstractConstraintViewModel : public IdentifiedObject<AbstractConstraintViewModel>
{
		Q_OBJECT

	public:
		AbstractConstraintViewModel(id_type<AbstractConstraintViewModel> id,
									QString name,
									ConstraintModel* model,
									QObject* parent);

		template<typename DeserializerVisitor>
		AbstractConstraintViewModel(DeserializerVisitor&& vis,
									ConstraintModel* model,
									QObject* parent):
			IdentifiedObject<AbstractConstraintViewModel>{vis, parent},
			m_model{model}
		{
			vis.writeTo(*this);
		}

		virtual AbstractConstraintViewModel* clone(id_type<AbstractConstraintViewModel> id,
												   ConstraintModel* cm,
												   QObject* parent) = 0;

		ConstraintModel* model() const
		{ return m_model; }

		bool isBoxShown() const;
		id_type<BoxModel> shownBox() const;

		void hideBox();
		void showBox(id_type<BoxModel> boxId);

	signals:
		void boxRemoved();
		void boxHidden();
		void boxShown(id_type<BoxModel> boxId);

	public slots:
		virtual void on_boxRemoved(id_type<BoxModel> boxId);


	private:
		// A view model cannot be constructed without a model
		// hence we are safe with a pointer
		ConstraintModel* m_model{};

		id_type<BoxModel> m_shownBox{};
};

/*
 * This file is part of the PulseView project.
 *
 * Copyright (C) 2013 Joel Holdsworth <joel@airwebreathe.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>

#include <QDoubleSpinBox>

#include "double.hpp"

using boost::optional;
using std::pair;

namespace pv {
namespace prop {

Double::Double(QString name,
	QString desc,
	int decimals,
	QString suffix,
	optional< pair<double, double> > range,
	optional<double> step,
	Getter getter,
	Setter setter) :
	Property(name, desc, getter, setter),
	decimals_(decimals),
	suffix_(suffix),
	range_(range),
	step_(step),
	spin_box_(nullptr)
{
}

QWidget* Double::get_widget(QWidget *parent, bool auto_commit)
{
	if (spin_box_)
		return spin_box_;

	if (!getter_)
		return nullptr;

	Glib::VariantBase variant = getter_();
	if (!variant.gobj())
		return nullptr;

	spin_box_ = new QDoubleSpinBox(parent);
	spin_box_->setDecimals(decimals_);
	spin_box_->setSuffix(suffix_);
	if (range_)
		spin_box_->setRange(range_->first, range_->second);
	if (step_)
		spin_box_->setSingleStep(*step_);

	update_widget();

	if (auto_commit)
		connect(spin_box_, SIGNAL(valueChanged(double)),
			this, SLOT(on_value_changed(double)));

	return spin_box_;
}

void Double::update_widget()
{
	if (!spin_box_)
		return;

	Glib::VariantBase variant = getter_();
	assert(variant.gobj());

	double value = Glib::VariantBase::cast_dynamic<Glib::Variant<double>>(
		variant).get();
	spin_box_->setValue(value);
}

void Double::commit()
{
	assert(setter_);

	if (!spin_box_)
		return;

	setter_(Glib::Variant<double>::create(spin_box_->value()));
}

void Double::on_value_changed(double)
{
	commit();
}

}  // namespace prop
}  // namespace pv

/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openflipper.org                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenFlipper.                                         *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
*                                                                            *
\*===========================================================================*/

#include <MillingSimulatorPlugin.hh>

#include <QFileDialog>

//#include <QInputDialog>
//#include <QtXml/QXmlSimpleReader>
//#include <QtXml/QDomDocument>
//#include "MillingSimulatorPlugin.hh"
//#include <MachineProvider.h>
//#include "MachineJsonParser.hh"
//#include "Plugin-TypeTriangleMesh/TriMeshBackup.hh"
//#include "GCodeSimulator.hh"
//#include <parserlib/parser.h>
//
//using namespace std;
//using namespace gpr;

//------------------------------------------------------------------------------

/** \brief Constructor
 *
 */
MillingSimulatorPlugin::MillingSimulatorPlugin():
   toolbox_(nullptr)
{

}

//------------------------------------------------------------------------------

QString MillingSimulatorPlugin::name() {
  return "MillingSimulator";
}

//------------------------------------------------------------------------------

QString MillingSimulatorPlugin::description() {
  return "Plugin to Simulate CNC Machines using G-Code";
}

void MillingSimulatorPlugin::initializePlugin() {
  toolbox_ = new MillingPluginToolbox();

  connect( toolbox_->machineLoadButton, SIGNAL(clicked()), this, SLOT(openMachineFileDialog()));
  connect( toolbox_->pbPlay, SIGNAL(clicked()), this, SLOT(play()));
  connect( toolbox_->pbNextFrame, SIGNAL(clicked()), this, SLOT(nextStep()));
  connect( toolbox_->gcodeloadbutton, SIGNAL(clicked()), this, SLOT(openGCodeFileDialog()));
	connect( toolbox_->xAxisSpinBox, SIGNAL(valueChanged ( double )), this, SLOT( setXAxis(double)));
	connect( toolbox_->yAxisSpinBox, SIGNAL(valueChanged ( double )), this, SLOT( setYAxis(double)));
	connect( toolbox_->zAxisSpinBox, SIGNAL(valueChanged ( double )), this, SLOT( setZAxis(double)));
	connect( toolbox_->bAxisSpinBox, SIGNAL(valueChanged ( double )), this, SLOT( setBAxis(double)));
	connect( toolbox_->wAxisSpinBox, SIGNAL(valueChanged ( double )), this, SLOT( setCAxis(double)));

	emit addToolbox(tr("Milling Simulator"),toolbox_);
}

void MillingSimulatorPlugin::openMachineFileDialog()
{
  toolbox_->machineTextEdit->setPlainText(QFileDialog::getOpenFileName(toolbox_, tr("Open Machine Specification"), "", tr("( *.xml)")));
//	initializeMachine(toolBox->machineTextEdit->toPlainText());

}

void MillingSimulatorPlugin::play()
{
	
//	if(simulator == nullptr)
//		simulator = new GCodeSimulator(*machine, *gcode);
//	simulator->simulate();

}


void MillingSimulatorPlugin::stop()
{


}


void MillingSimulatorPlugin::nextStep()
{
//	if (simulator == nullptr) {
//
//		simulator = new GCodeSimulator(*machine, *gcode);
//	}
//	simulator->simulateSingleStep();
}



void MillingSimulatorPlugin::openGCodeFileDialog()
{
	auto filePath = QFileDialog::getOpenFileName(toolbox_, 	tr("Open GCode File"), "", tr("(*.gcode *.cnc)"));

//	toolBox->gcodeTextEdit->setPlainText(filePath);
//
//	QFile jsonFile(toolBox->gcodeTextEdit->toPlainText());
//
//	//try open file
//	if (!jsonFile.open(QIODevice::ReadOnly))
//	{
//		throw jsonFile.error();;
//	}
//	gcode = new gcode_program(parse_gcode(jsonFile.readAll().toStdString()));

}


void MillingSimulatorPlugin::setXAxis(double value) {
//  const auto delta = value - x;
//  x = value;
//  machine->translate(QVector3D(delta, 0, 0));
//  updateModelMatrices();

}
void MillingSimulatorPlugin::setYAxis(double value) {
//  const auto delta = value - y;
//  y = value;
//  machine->translate(QVector3D(0, delta, 0));
//  updateModelMatrices();

}

void MillingSimulatorPlugin::setZAxis(double value) {
//  const auto delta = value - z;
//  z = value;
//  machine->translate(QVector3D(0, 0, delta));
//  updateModelMatrices();

}

void MillingSimulatorPlugin::setBAxis(double value) {
//  const auto delta = value - b;
//  b = value;
//  updateModelMatrices();

}

void MillingSimulatorPlugin::setCAxis(double value) {
//  const auto delta = value - c;
//  c = value;
//  updateModelMatrices();
}

//
//void MillingSimulatorPlugin::loadMachineMeshes(Machine* machine, QString basePath)
//{
//	if (machine == nullptr)
//		return;
//	if(toolBox == nullptr || toolBox->machineTextEdit == nullptr)
//		return;
//
//	///dfs over machine parts
//	QStack<Part*> stack;
//	stack.push_back(machine->root);
//	while(!stack.isEmpty()) {
//		auto item = stack.pop();
//		emit load(basePath + item->meshObjectPath,typeId("TriangleMesh"), item->objectId);
//		for(auto child : item->subparts)
//			stack.push_back(child);
//	}
//}
//
//void MillingSimulatorPlugin::initializeMachine(QString machineSpecificationPath) {
//	MachineProvider* provider = new MachineJsonParser(machineSpecificationPath);
//	machine = provider->GetMachine();
//	machine->translationConstant = 1/(double)10;
//	machine->setParent(this);
//    // machine->onChanged = [this](){updateModelMatrices();};
//	connect(machine, SIGNAL(changed()), this, SLOT(updateModelMatrices()));
//	// QTimer* testTimer = new QTimer(this);
//	// connect(testTimer, SIGNAL(timeout()), this, SLOT(updateModelMatrices()));
//	// testTimer->start(500);
// 	//get directory to machine file
//	auto filePath = toolBox->machineTextEdit->toPlainText();
//	auto index = filePath.lastIndexOf(QChar('/'));
//	loadMachineMeshes(machine, filePath.left(index));
//	emit updateView();
//}
//
//void MillingSimulatorPlugin::updateModelMatrices()
//{
//    ///dfs over machine parts
//    QStack<Part*> stack;
//    stack.push_back(machine->root);
//    while (!stack.isEmpty()) {
//        auto part = stack.pop();
//
//        auto mesh = PluginFunctions::triMeshObject(part->objectId);
//        auto translation = part->getTranslation() * machine->translationConstant;
//        mesh->manipulatorNode()->setTranslation(ACG::Vec3d(static_cast<double>(translation.x()), static_cast<double>(translation.y()), static_cast<double>(translation.z())));
//		emit updateView();
//        for (auto child : part->subparts)
//            stack.push_back(child);
//    }
//    emit updateView();
//}
//


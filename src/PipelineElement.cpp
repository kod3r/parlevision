#include "PipelineElement.h"

#include <QString>
#include <QDebug>

#include "Pipeline.h"
#include "RefCounted.h"
#include "Pin.h"

using namespace plv;

std::list<QString> PipelineElement::s_types;

PipelineElement::PipelineElement()
{
}

PipelineElement::~PipelineElement()
{
    if(m_parent != 0)
    {
        m_parent->remove( this );
    }
}

PipelineElement::PipelineElement(const PipelineElement &other)
{
}

void PipelineElement::setPipeline(Pipeline* parent)
{
    if(m_parent.getPtr() == parent)
    {
        // no effect
        return;
    }

    if(m_parent.getPtr() != 0)
    {
        // switching pipelines.
        // remove ourself from the old pipeline first
        m_parent->remove(this);
    }

    m_parent = parent;
}

void PipelineElement::addInputPin( IInputPin* pin )
{
    InputPinMap::iterator itr = m_inputPins.find( pin->getName() );
    if( itr != m_inputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }
    RefPtr<IInputPin> rpin( pin );
    m_inputPins.insert( std::make_pair( pin->getName(), rpin ));
}

void PipelineElement::addOutputPin( IOutputPin* pin )
{
    OutputPinMap::iterator itr = m_outputPins.find( pin->getName() );
    if( itr != m_outputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }
    RefPtr<IOutputPin> rpin( pin );
    m_outputPins.insert( std::make_pair( pin->getName(), rpin ));
}

IInputPin* PipelineElement::getInputPin( const QString& name ) const
{
    InputPinMap::const_iterator itr = m_inputPins.find( name );
    if( itr != m_inputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}

IOutputPin* PipelineElement::getOutputPin( const QString& name ) const
{
    OutputPinMap::const_iterator itr = m_outputPins.find( name );
    if( itr != m_outputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}

void PipelineElement::__process()
{
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr->second;
        in->scope();
    }

    this->process();

    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr->second;
        in->unscope();
    }
}



std::list<QString> PipelineElement::types()
{
    return PipelineElement::s_types;
}

int PipelineElement::registerType(QString typeName)
{
    qDebug() << "Registering PipelineElement" << typeName;
    PipelineElement::s_types.push_back( typeName );
    //return qRegisterMetaType<E>(typeName);
    return 0;
}

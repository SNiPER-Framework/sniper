/* Copyright (C) 2018
   Jiaheng Zou <zoujh@ihep.ac.cn> Tao Lin <lintao@ihep.ac.cn>
   Weidong Li <liwd@ihep.ac.cn> Xingtao Huang <huangxt@sdu.edu.cn>
   This file is part of SNiPER.
 
   SNiPER is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   SNiPER is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License
   along with SNiPER.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef SNIPER_PROPERTY_H
#define SNIPER_PROPERTY_H

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include "SniperKernel/SniperException.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

namespace bp = boost::python;

class Property
{
    public :

        Property(const std::string& key);

        virtual ~Property();

        //return the property key name
        const std::string& key() {return m_key; }

        //set property value as var
        virtual bool set(bp::object& var) = 0;

        //append var to vector/map property
        virtual bool append(bp::object& var);

        //show the value of the property
        void show() { show(0); }

        //print the value with indent
        virtual void show(int indent) = 0;

    protected :

        // offer a better look for show()
        void make_indent(int indent);

        const std::string  m_key;
};

template<typename T>
class SniperProperty : public Property
{
    public :

        SniperProperty(const std::string& key, T& var)
            : Property(key), m_var(var) {}

        bool set(bp::object& var)
        {
            bp::extract<T> _var(var);
            if ( _var.check() ) {
                m_var = _var();
                return true;
            }
            throw ContextMsgException(m_key + " : mismatched property type");
        }


        void show(int indent) {
            make_indent(indent);
            std::cout  << "[Var]" << std::setiosflags(std::ios::left)
                << std::setw(10) << m_key << " = " << m_var << std::endl;
        }

    private :

        T& m_var;
};

template<typename T>
class SniperProperty<std::vector<T> > : public Property
{
    public :

        SniperProperty(const std::string& key, std::vector<T>& var)
            : Property(key), m_var(var) {}

        bool set(bp::object& var) {
            m_var.clear();
            return this->append(var);
        }

        bool append(bp::object& var) {
            bp::extract<T> _var(var);
            if ( _var.check() ) {
                m_var.push_back(_var());
                return true;
            }
            bp::stl_input_iterator<T> begin(var), end;
            m_var.insert(m_var.end(), begin, end);
            return true;
        }

        void show(int indent) {
            make_indent(indent);
            std::cout << "[Var]" << std::setiosflags(std::ios::left)
                << std::setw(10) << m_key << " = [";
            if ( !m_var.empty() ) {
                std::cout << m_var[0];
                for (unsigned int i = 1; i < m_var.size(); ++i ) {
                    std::cout << ", " << m_var[i];
                }
            }
            std::cout << ']' << std::endl;
        }

    private :

        std::vector<T>& m_var;
};

template<typename K, typename V>
class SniperProperty<std::map<K, V> > : public Property
{
    public:

        SniperProperty(const std::string& key, std::map<K, V>& var)
            : Property(key), m_var(var) {}

        bool set(bp::object& var) {
            m_var.clear();
            return this->append(var);
        }

        bool append(bp::object& var) {
            bp::extract<bp::dict> dvar_(var);
            if ( dvar_.check() ) {
                bp::dict dvar = dvar_();
                bp::list iterkeys = (bp::list)dvar.iterkeys();
                bp::ssize_t dsize = bp::len(iterkeys);
                for(bp::ssize_t i = 0; i < dsize; ++i) {
                    bp::object kobj = iterkeys[i];
                    bp::extract<K> kvar(kobj);
                    bp::object vobj = dvar[kobj];
                    bp::extract<V> vvar(vobj);
                    m_var.insert(std::make_pair(kvar(), vvar()));
                }
            }
            return true;
        }

        void show(int indent) {
            make_indent(indent);
            std::cout << "[Var]" << std::setiosflags(std::ios::left)
                << std::setw(10) << m_key << " = {";
            if ( ! m_var.empty() ) {
                typename std::map<K, V>::iterator it = m_var.begin();
                std::cout << it->first << ':' << it->second;
                for ( ++it; it != m_var.end(); ++it ) {
                    std::cout << ", " << it->first << ':' << it->second;
                }
            }
            std::cout << '}' << std::endl;
        }

    private :

        std::map<K, V>& m_var;
};

#endif

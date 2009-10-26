//  prototype of a supercollider-synthdef-based synth prototype, implementation
//  Copyright (C) 2009 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#include <iostream>

#include <boost/filesystem/operations.hpp>

#include "sc_synth.hpp"
#include "sc_synth_prototype.hpp"

#include "server/synth_factory.hpp"

namespace nova
{

using namespace std;

void register_synthdefs(synth_factory & factory, std::vector<sc_synthdef> const & defs)
{
    foreach(sc_synthdef const & def, defs) {
        auto_ptr<sc_synth_prototype> sp(new sc_synth_prototype(def));
        factory.register_prototype(sp.get());
        sp.release();
    }
}

void sc_read_synthdefs_file(synth_factory & factory, path const & file)
{
    try {
        std::vector<sc_synthdef> defs = read_synthdef_file(file.string());
        register_synthdefs(factory, defs);
    }
    catch(std::exception & e)
    {
        cout << "Exception when parsing synthdef: " << e.what() << endl;
    }
}

void sc_read_synthdefs_dir(synth_factory & factory, path const & dir)
{
    using namespace boost::filesystem;

    if (!exists(dir))
        return;

    directory_iterator end;
    for (directory_iterator it(dir);
         it != end; ++it)
    {
        if (is_directory(it->status()))
            sc_read_synthdefs_dir(factory, it->path());
        else
            sc_read_synthdefs_file(factory, it->path());
    }
}

sc_synth_prototype::sc_synth_prototype(sc_synthdef const & sd):
    synth_prototype(sd.name()), synthdef(sd)
{
    typedef sc_synthdef::parameter_map_t::const_iterator iterator;

    for (iterator it = sd.parameter_map.begin(); it != sd.parameter_map.end(); ++it)
        slot_resolver::register_slot(it->first, it->second);
}


abstract_synth * sc_synth_prototype::create_instance(int node_id)
{
    sc_synth * synth = new sc_synth(node_id, this);

    return synth;
}


} /* namespace nova */

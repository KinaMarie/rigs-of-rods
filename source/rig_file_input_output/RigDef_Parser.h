/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2015 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

	Rigs of Rods is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3, as
	published by the Free Software Foundation.

	Rigs of Rods is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/**
	@file   RigDef_Parser.h
	@author Petr Ohlidal
	@date   12/2013
*/

#pragma once

#include "RigDef_Prerequisites.h"
#include "RigDef_File.h"
#include "RigDef_SequentialImporter.h"

#include <string>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>

namespace RigDef
{

/**
	@class  Parser
	@author Petr Ohlidal

	@brief Checks the rig-def file syntax and pulls data to File object

	ARCHITECTURE: 
		Every section/directive of the file should have it's own parser function. 
		If some sections are so similar that a single function could be used, 
		write a helper function and 2 wrappers for the sections.

	GENERAL RULE:
		The Parser should only check syntax and read data. Data transformation should be performed
		in another subsystem. There are cases when data need to be processed immediately, but those
		are exceptions. For example, the 'TractionControl/regulating_force' parameter has valid
		range 0-20 and any other values should be clamped to it - however, the parser should
		load the data as-is and leave the clamping to further processing.

	PRINCIPLE: 
		For every section/directive, there is a data-container struct defined in File.h.

		Every time a line of a particular section is parsed, an instance of the struct
		is saved into an array container in struct RigDef::File. There are exceptions to this rule.
		If the section is subject to adjustable defaults (like 'beams' to 'set_beam_defaults'),
		the default-data-container is attached to section-data-container.

		Optional sections (for example 'SlopeBrake') are represented by a dynamically allocated
		object. If the file doesn't contain the section, the pointer will be null.

		Whenever a directive is parsed, it's contents are stored to an instance of it's container
		struct, located in Parser.
		For every directive, there are two containers:
		* 'ror_*' represents game defaults as specified in documentation. Needed for resetting.
		* 'user_*' represent the last defaults specified in the .truck file.
		Again, there are exceptions to this rule.
	
*/

class Parser
{

public:

	struct Message
	{
		enum Type
		{
			TYPE_WARNING,
			TYPE_ERROR,
			TYPE_FATAL_ERROR,

			TYPE_INVALID = 0xFFFFFFFF
		};

		std::string      line;
		unsigned int     line_number;
		std::string      message;
		File::Section    section;
		File::Subsection subsection;
		Type             type;
		Ogre::String     module;
		
	};

	Parser();

	virtual ~Parser();

	void Prepare();

	void Finalize();

	void ParseLine(Ogre::String const & line);

	std::list<Message> const & GetMessages()
	{
		return m_messages;
	}

	boost::shared_ptr<RigDef::File> GetFile()
	{
		return m_definition;
	}

    SequentialImporter* GetSequentialImporter() { return &m_sequential_importer; }

    std::string ProcessMessagesToString();

    int GetMessagesNumErrors()   const { return m_messages_num_errors;   }
    int GetMessagesNumWarnings() const { return m_messages_num_warnings; }
    int GetMessagesNumOther()    const { return m_messages_num_other;    }

protected:

/* -------------------------------------------------------------------------- */
/*	Directive parsers                                                         */
/* -------------------------------------------------------------------------- */

	void ParseDirectiveAddAnimation(Ogre::String const & line);

	void ParseDirectiveDetacherGroup(Ogre::String const & line);

	void ParseDirectiveFlexbodyCameraMode(Ogre::String const & line);

	void ParseDirectivePropCameraMode(Ogre::String const & line);

	void ParseDirectiveSetBeamDefaults(Ogre::String const & line);

	void ParseDirectiveSetBeamDefaultsScale(Ogre::String const & line);

	void ParseDirectiveSetInertiaDefaults(Ogre::String const & line);

	void ParseDirectiveSetManagedMaterialsOptions(Ogre::String const & line);

	void ParseDirectiveSetNodeDefaults(Ogre::String const & line);
	void ParseDirectiveSetNodeDefaultsUnsafe(Ogre::String const & line);
	void VerifyAndProcessDirectiveSetNodeDefaults(Ogre::String const & line, float loadweight, float friction, float volume, float surface, unsigned int options);
	void LogParsedDirectiveSetNodeDefaultsData(Ogre::String const & line, float loadweight, float friction, float volume, float surface, unsigned int options);

/* -------------------------------------------------------------------------- */
/*	Section parsers                                                           */
/* -------------------------------------------------------------------------- */

	void ParseAirbrakes(Ogre::String const & line);

	void ParseAnimator(Ogre::String const & line);

	void ParseAntiLockBrakes(Ogre::String const & line);

	void ParseAuthor(Ogre::String const & line);

	void ParseAxles(Ogre::String const & line);

	void ParseBeams(Ogre::String const & line);

	void ParseBrakes(Ogre::String line);

	void ParseCameras(Ogre::String const & line);
	void ParseCamerasUnsafe(Ogre::String const & line);

	void ParseCameraRails(Ogre::String const & line);

	void ParseCinecam(Ogre::String const & line);

	void ParseCollisionBox(Ogre::String const & line);

	void ParseCommand(Ogre::String const & line);

	void ParseCommand2(Ogre::String const & line);

	void ParseContacter(Ogre::String const & line);

	void ParseCruiseControl(Ogre::String const & line);

	void ParseEngine(Ogre::String const & line);

	void ParseEngoption(Ogre::String const & line);

	void ParseEngturbo(Ogre::String const & line);

	void ParseExhaust(Ogre::String const & line);

	void ParseExtCamera(Ogre::String const & line);

	void ParseFileFormatVersion(Ogre::String const & line);

	void ParseFileinfo(Ogre::String const & line);

	void ParseFixes(Ogre::String const & line);

	void ParseFlare(Ogre::String const & line);

	void ParseFlare2(Ogre::String const & line);

	void ParseFlexbody(Ogre::String const & line);

	// FlexBodyWheels
	void ParseFlexBodyWheel(Ogre::String const & line);
	void ParseFlexBodyWheelUnsafe(Ogre::String const & line);
	void VerifyAndProcessFlexBodyWheel(Ogre::String const & line, FlexBodyWheel& def, int braking, int propulsion, char side);

	void ParseFusedrag(Ogre::String const & line);

	void ParseGlobals(Ogre::String const & line);

	void ParseGuid(Ogre::String const & line);

	void ParseGuiSettings(Ogre::String const & line);

	void ParseHelp(Ogre::String const & line);

	void ParseHook(Ogre::String const & line);

	void ParseHydros(Ogre::String const & line);

	void ParseLockgroups(Ogre::String const & line);

	void ParseManagedMaterials(Ogre::String const & line);

	void ParseMaterialFlareBindings(Ogre::String const & line);

	// MeshWheels
	void ParseMeshWheel(Ogre::String const & line);
	void ParseMeshWheelUnsafe(Ogre::String const & line);
	void VerifyAndProcessMeshWheel(Ogre::String const & line, MeshWheel& mesh_wheel_def, int braking, int propulsion, char side);

	// MeshWheels2
	void ParseMeshWheel2(Ogre::String const & line);
	void ParseMeshWheel2Unsafe(Ogre::String const & line);
	void VerifyAndProcessMeshWheel2(Ogre::String const & line, MeshWheel2& mesh_wheel_2_def, int braking, int propulsion, char side);

	void ParseMinimass(Ogre::String const & line);

	void ParseNode(Ogre::String const & line);

	void ParseNode2(Ogre::String const & line);

	void ParseNodeCollision(Ogre::String const & line);

	void ParseParticles(Ogre::String const & line);

	void ParsePistonprops(Ogre::String const & line);

	void ParseProps(Ogre::String const & line);

	void ParseRailGroups(Ogre::String const & line);

	void ParseRopables(Ogre::String const & line);

	void ParseRopes(Ogre::String const & line);

	void ParseRotators(Ogre::String const & line);

	void ParseRotators2(Ogre::String const & line);

	void ParseScrewprops(Ogre::String const & line);

	void ParseSetCollisionRange(Ogre::String const & line);

	void ParseSetSkeletonSettings(Ogre::String const & line);

	// Shocks
	void         ParseShock(Ogre::String const & line);
	void         ParseShockUnsafe(Ogre::String const & line);
	unsigned int ParseShockOptions(Ogre::String const & line, std::string const & options_str);
	void         LogParsedShockDataForChecking(Ogre::String const & line, Shock& shock);

	// Shocks2
	void         ParseShock2(Ogre::String const & line);
	void         ParseShock2Unsafe(Ogre::String const & line);
	unsigned int ParseShock2Options(Ogre::String const & line, std::string const & options_str);
	void         LogParsedShock2DataForChecking(Ogre::String const & line, Shock2& shock);

	void ParseSlidenodes(Ogre::String const & line);

	void ParseSlopeBrake(Ogre::String const & line);

	void ParseSoundsources(Ogre::String const & line);

	void ParseSoundsources2(Ogre::String const & line);

	void ParseSpeedLimiter(Ogre::String const & line);

	void ParseSubmesh(Ogre::String const & line);
	void ParseSubmeshUnsafe(Ogre::String const & line);

	void ParseSubmeshGroundModel(Ogre::String const & line);

	void ParseTies(Ogre::String const & line);

	void ParseTorqueCurve(Ogre::String const & line);

	void ParseTractionControl(Ogre::String const & line);

	void ParseTriggers(Ogre::String const & line);

	void ParseTurbojets(Ogre::String const & line);

	/**
	* Sections 'turboprops' & 'turboprops2'.
	*/
	void ParseTurboprops(Ogre::String const & line);

	void ParseVideoCamera(Ogre::String const & line);

	void ParseWheel(Ogre::String const & line);

	void ParseWheel2(Ogre::String const & line);

	void ParseWing(Ogre::String const & line);

/* -------------------------------------------------------------------------- */
/*	Utilities */
/* -------------------------------------------------------------------------- */

    /** The old, unsafe method of parsing, used in v0.38 + v0.4.0.7
    * @return -1 on error (error logged). Otherwise, number of arguments
    */
    int _ParseArgs(std::string const & line, Ogre::StringVector &args, unsigned min_num_args);

    /** Attempts to parse cab line. Returns true if successful.
    */
    bool _TryParseCab(Ogre::String const & line);

	unsigned int _ParseCabOptions(Ogre::String const & options_str);

	void _CheckInvalidTrailingText(Ogre::String const & line, boost::smatch const & results, unsigned int index);

	/** Keyword scan function. 
	*/
	File::Keyword IdentifyKeyword(Ogre::String const & line);

    /** Keyword scan utility function. 
	*/
    File::Keyword FindKeywordMatch(boost::smatch& search_results);

	/** Adds a message to parser report.
	*/
	void AddMessage(std::string const & line, Message::Type type, std::string const & message);

	/** Helper - parses stuff common to ROTATORS & ROTATORS2
	*/
	void _ParseRotatorsCommon(Rotator & rotator, boost::smatch & results, unsigned int inertia_start_index);

	/** Nodes and Nodes2 are unified with this parser.
	*/
	void _ParseSectionsNodesNodes2(Ogre::String const & line, bool is_version_2);

    /** The old, unsafe method of parsing, used in v0.38 + v0.4.0.7
	*/
    void _ParseNodesLegacyMethod(Ogre::String line, bool is_version_2);

    /** Print a log INFO message.
    */
    void _PrintNodeDataForVerification(Ogre::String& line, Ogre::StringVector& args, int num_args, Node& node);

    static void _TrimTrailingComments(std::string const & line_in, std::string & line_out);

    void _ImportLegacyFlexbodyForsetLine(Ogre::String const & line);

	/** Commands and Commands2 are unified with this parser.
	*/
	void _ParseSectionsCommandsCommands2(Ogre::String const & line, boost::regex const & regex, unsigned int format_version);

	Node::Ref _ParseNodeRef(std::string const & node_id_str);

	Node::Id _ParseOptionalNodeId(std::string const & node_id_str);

	void _ParseDirectiveAddAnimationMode(Animation & animation, Ogre::String mode_string);

	void _ParseDirectiveAddAnimationSource(Animation & animation, Ogre::String source_string);

	void _ParseCameraSettings(CameraSettings & camera_settings, Ogre::String input_str);

	void _ParseNodeOptions(unsigned int & options, const std::string & options_str);

	std::pair<bool, Ogre::String> GetModuleName(Ogre::String const & line);

	void SetCurrentModule(Ogre::String const & name);

	void _ExitSections(Ogre::String const & line);

	void RestoreRootModule();

	bool _IsCurrentModuleRoot()
	{
		return m_root_module == m_current_module;
	}

	/**
	* @return True if there could be more results, false if end was hit.
	*/
	bool _ParseOptionalInertia(Inertia & inertia, boost::smatch & results, unsigned int start_index);

/* -------------------------------------------------------------------------- */

	/* RoR defaults */

	boost::shared_ptr<Inertia>           m_ror_default_inertia;
	boost::shared_ptr<BeamDefaults>      m_ror_beam_defaults;
	boost::shared_ptr<NodeDefaults>      m_ror_node_defaults;
	float                                m_ror_minimass;
	SkeletonSettings                     m_ror_skeleton_settings;

	/* Data from user directives */
	/* Each affected section-struct has a shared_ptr to it's respective defaults */
	boost::shared_ptr<Inertia>           m_user_default_inertia;
	boost::shared_ptr<BeamDefaults>      m_user_beam_defaults;
	boost::shared_ptr<NodeDefaults>      m_user_node_defaults;
	int                                  m_current_detacher_group;
	ManagedMaterialsOptions              m_current_managed_material_options;

	/* Parser state */
	boost::shared_ptr<File::Module>      m_root_module;
	boost::shared_ptr<File::Module>      m_current_module;

	File::Section                        m_current_section;        ///< Parser state.
	File::Subsection                     m_current_subsection;     ///< Parser state.
	bool                                 m_in_block_comment;       ///< Parser state.
	bool                                 m_in_description_section; ///< Parser state.
    bool                                 m_any_named_node_defined; ///< Parser state.
	boost::shared_ptr<Submesh>           m_current_submesh;        ///< Parser state.
	boost::shared_ptr<CameraRail>        m_current_camera_rail;    ///< Parser state.
	boost::shared_ptr<Flexbody>          m_last_flexbody;
	unsigned int                         m_num_contiguous_blank_lines; ///< Parser state; Num. blank lines right above the current one

    SequentialImporter                   m_sequential_importer;

	unsigned int                         m_current_line_number; ///< Only for reports. Initialised to 1
	boost::shared_ptr<RigDef::File>      m_definition;
	std::list<Message>                   m_messages;
    int                                  m_messages_num_errors;
    int                                  m_messages_num_warnings;
    int                                  m_messages_num_other;
};

} // namespace RigDef

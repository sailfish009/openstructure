#ifndef OST_ENTITY_TO_DENSITY_HH
#define OST_ENTITY_TO_DENSITY_HH

#import <ost/mol/entity_view.hh>
#import <ost/img/map.hh>

#include <ost/mol/alg/module_config.hh>


namespace ost { namespace mol { namespace alg {

/// \brief type of density being created by the EntityToDensity function
enum DensityType
{
  SCATTERING_FACTORS,
  ROSETTA_HIGH_RESOLUTION,
  ROSETTA_LOW_RESOLUTION
};

/// \brief create a density representation of an entity in a density map
///
/// This function creates a density representation of the entity provided by
/// the user in a density map, also provided by the user. The user can choose
/// the type of density of the output map:
///
/// SCATTERING_FACTORS    density, generated in Fourier space using the
///                       correct scattering factors for the elements
///                       involved.
/// ROSETTA_HIGH_RESOLUTION gaussian spheres in real space to represent
///                         density, one per atom, see Dimaio et al.,
///                         Refinement of Protein Structures into Low-Resolution
///                         Density Maps Using Rosetta. Journal of Molecular
///                         Biology (2009) pp. 1-10
/// ROSETTA_LOW_RESOLUTION guassian spheres in real space to represent
///                        density, one per residue. See reference above. Only
///                        useful at low resolution
///
/// The user can also choose if the density map should be cleared of its
/// previous content before creating the density representation.
///
/// The first two types of density are generated in Fourier space. In order to
/// avoid artifacts in the final density representation, the function avoids
/// sharp frequency cutoffs by applying a Gaussian falloff. The user must
/// provide the resolutions at which the cutoff should begin and end, as opposed
/// to a single resolution cutoff value. For the other two density types, which
/// are generated in real space, the average of the two falloff values is used
/// as resolution parameter.
///
/// This function will only create a density represenation of the entities
/// (or portion of entities ) that fall within the borders of the map.
/// The user must take care that this condition is verified for all
/// entities for which he wants a representation.
///
void DLLEXPORT_OST_MOL_ALG EntityToDensity(const mol::EntityView& entity_view,
                                        img::MapHandle& map,
                                        const DensityType& density_type,
                                        Real falloff_start,
                                        Real falloff_end,
                                        bool clear_map_flag = false,
                                        Real source_wavelength = 1.5418);

}}} // ns

#endif // OST_ENTITY_TO_DENSITY

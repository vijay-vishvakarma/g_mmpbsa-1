/*
 * This file is part of the GROMACS molecular simulation package.
 *
 * Copyright 2019- The GROMACS Authors
 * and the project initiators Erik Lindahl, Berk Hess and David van der Spoel.
 * Consult the AUTHORS/COPYING files and https://www.gromacs.org for details.
 *
 * GROMACS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * GROMACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GROMACS; if not, see
 * https://www.gnu.org/licenses, or write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 *
 * If you want to redistribute modifications to GROMACS, please
 * consider that scientific software is very special. Version
 * control is crucial - bugs must be traceable. We will be happy to
 * consider code for inclusion in the official distribution, but
 * derived work must not be called official GROMACS. Details are found
 * in the README & COPYING files - if they are missing, get the
 * official version at https://www.gromacs.org.
 *
 * To help us fund GROMACS development, we humbly ask that you cite
 * the research papers on the package. Check out https://www.gromacs.org.
 */
/*! \libinternal \file
 * \brief
 * Declares density similarity measures and their derivatives.
 *
 * \author Christian Blau <blau@kth.se>
 * \inlibraryapi
 * \ingroup module_math
 */
#ifndef GMX_MATH_DENSITYFIT_H
#define GMX_MATH_DENSITYFIT_H

#include <memory>

#include "gromacs/mdspan/extensions.h"
#include "gromacs/utility/real.h"

namespace gmx
{

template<typename>
class ArrayRef;

/*! \brief
 * The methods that determine how two densities are compared to one another.
 */
enum class DensitySimilarityMeasureMethod : int
{
    /*! \brief Measure similarity between densities as normalized inner product of their
     * voxel values.
     *
     * \f[
     *      \mathrm{Similarity}(\rho_{\mathrm{r}},\rho_{\mathrm{c}}) =
     *           \frac{1}{N_\mathrm{voxel}}/\sum_{v=1}^{N_\mathrm{voxel}} \rho^v_{\mathrm{r}}
     * \rho^v_{\mathrm{c}}
     * \f]
     */
    innerProduct,

    /*! \brief Measure similarity between densities by negative relative entropy.
     * \note Voxels with negative values are ignored.
     *
     * \f[
     *      \mathrm{Similarity}(\rho_{\mathrm{r}},\rho_{\mathrm{c}}) =
     *           \sum_{v=1}^{N_\mathrm{voxel}}
     *              \rho^v_{\mathrm{r}} (\log(\rho^v_{\mathrm{c}}) - \log(\rho^v_{\mathrm{r}}))
     * \f]
     */
    relativeEntropy,

    /*! \brief Measure similarity between densities by cross correlation.
     *
     * \f[
     *      \mathrm{Similarity}(\rho_{\mathrm{r}},\rho_{\mathrm{c}}) =
     *           \frac{\sum_{v}\left((\rho_{\mathrm{r}} - \bar{\rho}_{\mathrm{r}})(\rho_{\mathrm{c}} - \bar{\rho}_{\mathrm{c}})\right)}
     *           {\sqrt{\sum_v(\rho_{\mathrm{r}} - \bar{\rho}_{\mathrm{r}})^2 \sum_v (\rho_{\mathrm{c}} - \bar{\rho}_{\mathrm{c}})^2}}
     * \f]
     */
    crossCorrelation,
    Count,
};

/* Forward declaration of implementation class outside class to allow
 * choose implementation class during construction of the DensitySimilarityMeasure*/
class DensitySimilarityMeasureImpl;

/*! \libinternal \brief
 *  Measure similarity and gradient between densities.
 */
class DensitySimilarityMeasure
{
public:
    //! a three-dimensional const view into density data
    using density = basic_mdspan<const float, dynamicExtents3D>;
    /*! \brief Chose comparison method and set reference density.
     * \param[in] method defines how densities are compared to one another
     * \param[in] referenceDensity
     * \throws NotImplementedError if method is not known
     */
    DensitySimilarityMeasure(DensitySimilarityMeasureMethod method, density referenceDensity);
    ~DensitySimilarityMeasure();
    //! Copy constructor
    DensitySimilarityMeasure(const DensitySimilarityMeasure& other);
    //! Copy assignment
    DensitySimilarityMeasure& operator=(const DensitySimilarityMeasure& other);
    //! Move constructor
    DensitySimilarityMeasure(DensitySimilarityMeasure&& other) noexcept;
    //! Move assignment
    DensitySimilarityMeasure& operator=(DensitySimilarityMeasure&& other) noexcept;

    /*! \brief Derivative of the density similarity measure at all voxels.
     * \param[in] comparedDensity the variable density
     * \returns density similarity measure derivative
     */
    density gradient(density comparedDensity);
    /*! \brief Similarity between reference and compared density.
     * \param[in] comparedDensity the variable density
     * \returns density similarity
     */
    real similarity(density comparedDensity);

private:
    std::unique_ptr<DensitySimilarityMeasureImpl> impl_;
};

/*! \brief Divide all values of a view by a constant so that the sum of
 *         all its positive values is one.
 *
 * \note Does nothing if there are no positive values
 *
 * \param[in,out] data the input data to be normalized
 *
 */
void normalizeSumPositiveValuesToUnity(ArrayRef<float> data);

} // namespace gmx

#endif

/*
 * Copyright (C) 2000-2004  Object Oriented Parallel Simulation Engine (OOPSE) project
 * 
 * Contact: oopse@oopse.org
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * All we ask is that proper credit is given for our work, which includes
 * - but is not limited to - adding the above copyright notice to the beginning
 * of your source code files, and to any copyright notice that you may distribute
 * with programs based on this work.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/**
 * @file Vector3d.hpp
 * @author Teng Lin
 * @date 09/14/2004
 * @version 1.0
 */
 
#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

#include <cassert>
#include <cmath>

namespace oopse {

    /**
     * @class Vector3d Vector3d.hpp "math/Vector3d.hpp"
     * @brief Fix length vector class
     */
    class Vector3d{
        public:

            /** default constructor */
            inline Vector3d(){
                data_[0] = 0.0;
                data_[1] = 0.0;
                data_[2] = 0.0;
            }

            /** Constructs and initializes a Vector3d from an array */            
            inline Vector3d( double* v) {
                data_[0] = v[0];
                data_[1] = v[1];
                data_[2] = v[2];
            }

            /** Constructs and initializes a Vector3d from x, y, z coordinates */            
            inline Vector3d( double x, double y, double z) {
                data_[0] = x;
                data_[1] = y;
                data_[2] = z;
            }

            /** 
             * Returns reference of ith element.
             * @return reference of ith element
             * @param i index
             */
            inline double& operator[](unsigned int  i) {
                assert( i < 3);
                return data_[i]
            }

            /** 
             * Returns reference of ith element.
             * @return reference of ith element
             * @param i index
             */
            inline double& operator()(unsigned int  i) {
                assert( i < 3);
                return data_[i]
            }

            /** 
             * Returns constant reference of ith element.
             * @return reference of ith element
             * @param i index
             */
            inline  const double& operator[](unsigned int i) const {
                assert( i < 3);
                return data_[i]
            }

            /** 
             * Returns constant reference of ith element.
             * @return reference of ith element
             * @param i index
             */
            inline  const double& operator()(unsigned int i) const {
                assert( i < 3);
                return data_[i]
            }
            /**
             * Retunrs reference of the first element of Vector3.
             * @return reference of the first element of Vector3
             */
            inline double& x() {  return data_[0];}

            /**
             * Retunrs the first element of Vector3.
             * @return  the first element of Vector3
             */
            inline double x() const {  return data_[0];}

            /**
             * Retunrs reference of the second element of Vector3.
             * @return reference of the second element of Vector3
             */
            inline double& y() {  return data_[1];}

            /**
             * Retunrs  the second element of Vector3.
             * @return c the second element of Vector3
             */
            inline double y() const {  return data_[1];}

            /**
             * Retunrs reference of the third element of Vector3.
             * @return reference of the third element of Vector3
             */
            inline double& z() {  return data_[2];}

            /**
             * Retunrs  the third element of Vector3.
             * @return f the third element of Vector3
             */
            inline double z() const {  return data_[2];}

            /**
             * Returns multiplication of matrix and vector
             * @return multiplication of matrix and vector
             * @param m matrix
             * @param v vector
             */
            /** Negates the value of this vector in place. */           
            inline void negate() {
                data_[0] = -data_[0];
                data_[1] = -data_[1];
                data_[2] = -data_[2];
            }

            /**
            * Sets the value of this vector to the negation of vector v1.
            * @param v1 the source vector
            */
            inline void negate(const Vector3d& v1) {
                data_[0] = -v1.data_[0];
                data_[1] = -v1.data_[1];
                data_[2] = -v1.data_[2];
            }
            
            /**
            * Sets the value of this vector to the sum of itself and v1 (*this += v1).
            * @param v1 the other vector
            */
            inline void add( const Vector3d& v1 ) {
                data_[0] += v1.data_[0];
                data_[1] += v1.data_[1];
                data_[2] += v1.data_[2];
            }

            /**
            * Sets the value of this vector to the sum of v1 and v2 (*this = v1 + v2).
            * @param v1 the first vector
            * @param v2 the second vector
            */
            inline void add( const Vector3d& v1, const Vector3d& v2 ) {
                data_[0] = v1.data_[0] + v2.data_[0];
                data_[1] = v1.data_[1] + v2.data_[1];
                data_[2] = v1.data_[2] + v2.data_[2];
            }

            /**
            * Sets the value of this vector to the difference  of itself and v1 (*this -= v1).
            * @param v1 the other vector
            */
            inline void sub( const Vector3d& v1 ) {
                data_[0] -= v1.data_[0];
                data_[1] -= v1.data_[1];
                data_[2] -= v1.data_[2];
            }

            /**
            * Sets the value of this vector to the difference of vector v1 and v2 (*this = v1 - v2).
            * @param v1 the first vector
            * @param v2 the second vector
            */
            inline void sub( const Vector3d& v1, const Vector3d  &v2 ){
                data_[0] = v1.data_[0] - v2.data_[0];
                data_[1] = v1.data_[1] - v2.data_[1];
                data_[2] = v1.data_[2] - v2.data_[2];
            }

            /**
            * Sets the value of this vector to the scalar multiplication of itself (*this *= s).
            * @param s the scalar value
            */
            inline void mul( double s ) {
                data_[0] *= s;
                data_[1] *= s;
                data_[2] *= s;
            }

            /**
            * Sets the value of this vector to the scalar multiplication of vector v1  
            * (*this = s * v1).
            * @param s the scalar value
            * @param v1 the vector
            */
            inline void mul( double s, const Vector3d& v1 ) {
                data_[0] = s * v1.data_[0];
                data_[1] = s * v1.data_[1];
                data_[2] = s * v1.data_[2];
            }

            /**
            * Sets the value of this vector to the scalar division of itself  (*this /= s ).
            * @param s the scalar value
            */             
            inline void div( double s) {
                data_[0] /= s;
                data_[1] /= s;
                data_[2] /= s;
            }

            /**
            * Sets the value of this vector to the scalar division of vector v1  (*this = v1 / s ).
            * @paran v1 the source vector
            * @param s the scalar value
            */                         
            inline void div( const Vector3d& v1, double s ) {
                data_[0] = v1.data_[0] / s;
                data_[1] = v1.data_[1] /s;
                data_[2] = v1.data_[2] /s;
            }

            /** @see #add */
            inline void operator +=( const Vector3d& v1 ) {
                data_[0] += v1.data_[0];
                data_[1] += v1.data_[1];
                data_[2] += v1.data_[2];
            }

            /** @see #sub */
            inline void operator -=( const Vector3d& v1 ) {
                data_[0] -= v1.data_[0];
                data_[1] -= v1.data_[1];
                data_[2] -= v1.data_[2];
            }

            /** @see #mul */
            inline void operator *=( double s) {
                data_[0] *= s;
                data_[1] *= s;
                data_[2] *= s;
            }

            /** @see #div */
            inline void operator /=( double s ) {
                data_[0] /= s;
                data_[1] /= s;
                data_[2] /= s;
            }

            /**
             * Returns the length of this vector.
             * @return the length of this vector
             */
             inline double length() {
                return sqrt(data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2]);  
            }
            
            /**
             * Returns the squared length of this vector.
             * @return the squared length of this vector
             */
             inline double lengthSquared() {
                return data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2];  
            }
            
            /** Normalizes this vector in place */
            inline void normalize() {
                double len;

                len = length();
                
                //if (len == 0)
                //    throw Exception("");

                *this /= len;
            }

            /** unary minus*/
            friend inline Vector3d operator -(const Vector3d& v1);
            
            /**
             * Return the sum of two vectors  (v1 - v2). 
             * @return the sum of two vectors
             * @param v1 the first vector
             * @param v2 the second vector
             */              
            friend inline Vector3d operator +( const Vector3d& v1 const Vector3d& v2) const;

            /**
             * Return the difference of two vectors  (v1 - v2). 
             * @return the difference of two vectors
             * @param v1 the first vector
             * @param v2 the second vector
             */              
            friend inline Vector3d operator -(const Vector3d& v1 const Vector3d& v2) const;

            /**
             * Returns the vaule of scalar multiplication of this vector v1 (v1 * r). 
             * @return  the vaule of scalar multiplication of this vector
             * @param v1 the source vector
             * @param s the scalar value
             */                
            friend inline Vector3d operator * ( const Vector3d& v1, double s);

            /**
             * Returns the vaule of scalar multiplication of this vector v1 (v1 * r). 
             * @return  the vaule of scalar multiplication of this vector
             * @param s the scalar value
             * @param v1 the source vector
             */                          
            friend inline Vector3d operator * ( double s, const Vector3d& v1 );

            /**
             * Returns the  value of division of a vector by a scalar. 
             * @return  the vaule of scalar division of this vector
             * @param v1 the source vector
             * @param s the scalar value
             */
            friend inline Vector3d operator /( const Vector3d& v1, double s ) const;
            
            /**
             * Returns the  value of division of a vector by a scalar. 
             * @return  the vaule of scalar division of this vector
             * @param s the scalar value
             * @param v1 the source vector
             */
            friend inline Vector3d operator /( double s, const Vector3d& v1 ) const;

            /** */     
            friend inline bool epsilonEqual( const Vector3d& v1, const Vector3d& v2); 

            /**
             * Returns the dot product of two Vectors
             * @param v1 first vector
             * @param v2 second vector
             * @return the dot product of v1 and v2
             */
            friend inline double dot( const Vector3d& v1, const Vector3d& v2 ); 
                   
            /**
             * Returns the cross product of two Vectors
             * @param v1 first vector
             * @param v2 second vector
             * @return the cross product  of v1 and v2
             * @see #vector::dot
             */
            friend inline Vector3d cross( const Vector3d& v1, const Vector3d& v2 );
            
            /**
             * Returns the distance between  two Vectors
             * @param v1 first vector
             * @param v2 second vector
             * @return the distance between v1 and v2
             */
            friend inline double distance( const Vector3d& v1, const Vector3d& v2 );

            /**
             * Returns the squared distance between  two Vectors
             * @param v1 first vector
             * @param v2 second vector
             * @return the squared distance between v1 and v2
             */
            friend inline double distanceSquare( const Vector3d& v1, const Vector3d& v2 );
            
            /**
             * Write to an output stream
             */
            friend std::ostream &operator<< ( std::ostream& o, const Vector3d& v1 );
            
        private:
            double data_[3];
        
    };

    Vector3d operator -(const Vector3d& v1){
        Vector3d tmp(v1);
        return tmp.negate();
    }

    Vector3d operator +(const Vector3d& v1, const Vector3d& v2) {
        Vector3d result;

        result.data_[0] = v1.data_[0] + v2.data_[0]; 
        result.data_[1] = v1.data_[1] + v2.data_[1]; 
        result.data_[2] = v1.data_[2] + v2.data_[2]; 

        return result;        
    }

    Vector3d operator -(const Vector3d& v1, const Vector3d& v2) {
        Vector3d result;

        result.data_[0] = v1.data_[0] - v2.data_[0]; 
        result.data_[1] = v1.data_[1] - v2.data_[1]; 
        result.data_[2] = v1.data_[2] - v2.data_[2]; 

        return result;        
    }
             
    Vector3d operator * ( const Vector3d& v1, double s) {       
        Vector3d result;

        result.data_[0] = v1.data_[0] * s; 
        result.data_[1] = v1.data_[1] * s; 
        result.data_[2] = v1.data_[2] * s; 

        return result;           
    }

    Vector3d operator * ( double s, const Vector3d& v1 ) {
        return v1 * s;
    }

    Vector3d operator / ( const Vector3d& v1, double s) {       
        Vector3d result;

        result.data_[0] = v1.data_[0] / s; 
        result.data_[1] = v1.data_[1] / s; 
        result.data_[2] = v1.data_[2] / s; 

        return result;           
    }
    
    Vector3d operator / ( double s, const Vector3d& v1 ) {
        return v1 / s;
    }

    
    bool epsilonEqual( const Vector3d& v1, const Vector3d& v2 ) {

    }

    double dot( const Vector3d& v1, const Vector3d& v2 ) {
        return v1.data_[0] * v2.data_[0] +
                   v1.data_[1] * v2.data_[1] +
                   v1.data_[2] * v2.data_[2]; 
    }

    Vector3d cross( const Vector3d& v1, const Vector3d& v2 ) {
        Vector3d result;

        result.data_[0] = v1.data_[1] * v2.data_[2] - v1.data_[2] * v2.data_[1] ;
        result.data_[1] = v1.data_[2] * v2.data_[0] - v1.data_[0] * v2.data_[2] ;
        result.data_[2] = v1.data_[0] * v2.data_[1] - v1.data_[1] * v2.data_[0];      

        return result;
    }
    
    double distance( const Vector3d& v1, const Vector3d& v2 ) {
        Vector3d tempVector = v1 - v2;
        return tempVector.length();
    }

    double distanceSquare( const Vector3d& v1, const Vector3d& v2 ) {
        Vector3d tempVector = v1 - v2;
        return tempVector.lengthSquare();
    }

    std::ostream &operator<< ( std::ostream& o, const Vector3d& v1 ) {
        o << " x = " << v1.data_[0] <<" y = " << v1.data_[1] << " z = " << v1.data_[2];
        return o;        
    }

    
}


#endif

module libresbos
    use iso_c_binding

    private
    public :: resbos

    ! Yes, include is a keyword in Fortran !
    include "ResBos_cdef.f90"

    ! We'll use a Fortran type to represent a C++ class here, in an opaque manner
    type resbos
        private
        type(c_ptr) :: ptr ! pointer to the ResBos class
    contains
        ! We can bind some functions to this type, allowing for a cleaner syntax.
#ifdef __GNUC__
        procedure :: delete => delete_resbos_polymorph ! Destructor for gfortran
#else
        final :: delete_resbos ! Destructor
#endif

        ! Function member
        procedure :: mesq => matrix_element
    end type

    ! This function will act as the constructor for resbos type
    interface resbos
        procedure create_resbos
    end interface

contains ! Implementation of the functions. We just wrap the C function here.
    function create_resbos(str)
        implicit none
        type(resbos) :: create_resbos
        character(len=*), intent(in) :: str
        character(len=1, kind=C_CHAR) :: c_str(len_trim(str) + 1)
        integer :: length, i

        ! Converting Fortran string to C string
        length = len_trim(str)
        do i = 1, length
            c_str(i) = str(i:i)
        end do
        c_str(length + 1) = C_NULL_CHAR

        create_resbos%ptr = create_resbos_c(c_str)
    end function

    subroutine delete_resbos(this)
        implicit none
        type(resbos) :: this
        call delete_resbos_c(this%ptr)
    end subroutine

    ! Bounds procedure needs to take a polymorphic (class) argument
    subroutine delete_resbos_polymorph(this)
        implicit none
        class(resbos) :: this
        call delete_resbos_c(this%ptr)
    end subroutine

    double precision function matrix_element(this, x)
        implicit none
        class(resbos), intent(in) :: this
        double precision, allocatable, intent(in) :: x(:)
        matrix_element = matrix_element_c(this%ptr, x)
    end function
end module

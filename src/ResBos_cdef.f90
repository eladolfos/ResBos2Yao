! C functions declaration
interface
    function create_resbos_c(filename) bind(C, name="CreateResbos")
        use iso_c_binding
        implicit none
        type(c_ptr) :: create_resbos_c
        character(len=1,kind=c_char) :: filename(*)
    end function

    subroutine delete_resbos_c(resbos) bind(C, name="DeleteResbos")
        use iso_c_binding
        implicit none
        type(c_ptr), value :: resbos
    end subroutine

    function matrix_element_c(resbos, x) bind(C, name="MatrixElement")
        use iso_c_binding
        implicit none
        real(c_double) :: matrix_element_c
        type(c_ptr), intent(in), value :: resbos
        real(c_double), dimension(*), intent(in) :: x
    end function
end interface


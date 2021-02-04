----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02/04/2021 12:20:55 AM
-- Design Name: 
-- Module Name: strobe_inv - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity strobe_inv is
    Port ( strobe : in STD_LOGIC;
            rst    : in STD_LOGIC;
           inv_out : out STD_LOGIC);
end strobe_inv;

architecture Behavioral of strobe_inv is
    signal tmp : std_logic;
begin

p1 : process (rst, strobe)
begin
    if rst = '1' then
        tmp <= '1';
    elsif rising_edge(strobe) then
        tmp <= not tmp;
    end if;
end process;

inv_out <= tmp;


end Behavioral;
